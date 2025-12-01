#include "server/api/socket.h"
#include "server/api/response.h"
#include "server/queue.h"
#include "server/sessions/map.h"
#include "server/sessions/checker.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

volatile sig_atomic_t stop_loop = 0;
server_socket_conn_t *ssc = NULL;

#ifdef __APPLE__
#include <TargetConditionals.h>
#include <sys/event.h>
#endif

#ifdef __linux__
#include <sys/epoll.h>
int32_t epoll_fd = -1;
#endif


void init_server_socket_conn(uint32_t port, bool verbose) {
    if (ssc)
        return;

    ssc = (server_socket_conn_t *)malloc(sizeof(server_socket_conn_t));
    if (!ssc)
        return;

    int32_t server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("Socket creation failed");
        free(ssc);
        ssc = NULL;
        return;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server);
        free(ssc);
        ssc = NULL;
        return;
    }

    if (listen(server, MAX_SOCKET_CONNECTIONS) < 0) {
        perror("Listen failed");
        close(server);
        free(ssc);
        ssc = NULL;
        return;
    }

    ssc->port = port;
    ssc->verbose = verbose;
    ssc->server = server;

    if (verbose)
        printf("Server now running at port %d\n", port);
}

void _handle_sigint(int sig) {
    (void)sig;
    stop_loop = 1;
}

int32_t _set_nonblocking(int32_t fd) {
    int32_t flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void server_loop(void (*handle_input)(int client_fd)) {
    if (!ssc)
        return;

    stop_loop = 0;
    signal(SIGINT, _handle_sigint);
    signal(SIGPIPE, SIG_IGN);

#ifdef __linux__
    if (epoll_fd >= 0) return;

    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) { perror("epoll_create1 failed"); return; }

    if (_set_nonblocking(ssc->server) < 0)
        perror("Failed to set server non-blocking");

    struct epoll_event ev = {0};
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = ssc->server;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ssc->server, &ev) < 0) {
        perror("epoll_ctl ADD server failed");
        close(epoll_fd);
        return;
    }

    struct epoll_event events[MAX_EVENTS];

    while (!stop_loop) {
        int32_t n = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
        if (n < 0) { if (errno == EINTR) continue; perror("epoll_wait failed"); break; }

        clear_idle_connection_sessions(epoll_fd);

        for (int32_t i = 0; i < n; i++) {
            int32_t fd = events[i].data.fd;
            uint32_t evs = events[i].events;

            // New connection
            if (fd == ssc->server && ssc && ssc->server >= 0) {
                for (;;) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int32_t client_fd = accept(ssc->server, (struct sockaddr*)&client_addr, &client_len);
                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        if (errno == EINTR || errno == ECONNABORTED) continue;
                        perror("accept failed");
                        break;
                    }

                    if (_set_nonblocking(client_fd) < 0 ||
                        fcntl(client_fd, F_SETFD, FD_CLOEXEC) < 0) {
                        close(client_fd);
                        continue;
                    }

                    struct epoll_event client_ev = {0};
                    client_ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
                    client_ev.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) < 0) {
                        perror("epoll_ctl ADD client failed");
                        close(client_fd);
                        continue;
                    }

                    add_connected_session(client_fd);

                    if (ssc->verbose)
                        printf("New client connected: fd=%d\n", client_fd);
                }
                continue;
            }

            // Client closed / error
            if (evs & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                destroy_connected_session(fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                if (ssc->verbose)
                    printf("Client disconnected: fd=%d\n", fd);
                continue;
            }

            handle_input(fd);
        }
    }

    close(epoll_fd);
    epoll_fd = -1;

#elif __APPLE__
    int kq = kqueue();
    if (kq < 0) { perror("kqueue failed"); return; }

    struct kevent change;
    EV_SET(&change, ssc->server, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(kq, &change, 1, NULL, 0, NULL) < 0) {
        perror("kevent ADD server failed");
        close(kq);
        return;
    }

    struct kevent events[MAX_EVENTS];

    while (!stop_loop) {
        int n = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (n < 0) { if (errno == EINTR) continue; perror("kevent failed"); break; }

        for (int i = 0; i < n; i++) {
            int fd = (int)events[i].ident;

            // New client connection
            if (fd == ssc->server) {
                for (;;) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(ssc->server, (struct sockaddr*)&client_addr, &client_len);
                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        if (errno == EINTR || errno == ECONNABORTED) continue;
                        perror("accept failed");
                        break;
                    }

                    fcntl(client_fd, F_SETFL, O_NONBLOCK);
                    fcntl(client_fd, F_SETFD, FD_CLOEXEC);

                    struct kevent client_ev;
                    EV_SET(&client_ev, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    if (kevent(kq, &client_ev, 1, NULL, 0, NULL) < 0) {
                        perror("kevent ADD client failed");
                        close(client_fd);
                        continue;
                    }

                    add_connected_session(client_fd);
                    if (ssc->verbose)
                        printf("New client connected: fd=%d\n", client_fd);
                }
                continue;
            }

            if (events[i].flags & EV_EOF) {
                destroy_connected_session(fd);
                close(fd);
                continue;
            }

            handle_input(fd);
        }
    }

    close(kq);
#endif
}

void close_server_socket_conn() {
    if (!ssc)
        return;

    if (ssc->verbose) {
        puts("\nClosing server...");
    }

    if (epoll_fd >= 0) {
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ssc->server, NULL);
        close(epoll_fd);
        epoll_fd = -1;
    }

    shutdown(ssc->server, SHUT_RDWR);
    close(ssc->server);
    if (ssc->verbose)
        printf("Server socket closed: fd=%d\n", ssc->server);

    free(ssc);
    ssc = NULL;
}
