#include "api/socket.h"
#include "api/response.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>


volatile sig_atomic_t stop_loop = 0;
server_socket_conn_t *ssc = NULL;
int32_t epoll_fd = -1;


void init_server_socket_conn(uint32_t port, uint8_t verbose)
{
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

void server_loop(thread_pool_t *pool)
{
    if (!ssc || epoll_fd >= 0)
        return;

    stop_loop = 0;
    signal(SIGINT, _handle_sigint);
    signal(SIGPIPE, SIG_IGN);

    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1 failed");
        return;
    }

    if (_set_nonblocking(ssc->server) < 0)
        perror("Failed to set server non-blocking");

    struct epoll_event ev = {0};
    ev.events = EPOLLIN;
    ev.data.fd = ssc->server;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ssc->server, &ev) < 0)
    {
        perror("epoll_ctl ADD server failed");
        close(epoll_fd);
        return;
    }

    struct epoll_event events[MAX_EVENTS];

    while (!stop_loop)
    {
        int32_t n = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
        if (n < 0)
        {
            if (errno == EINTR)
                continue;
            perror("epoll_wait failed");
            break;
        }

        for (int32_t i = 0; i < n; i++)
        {
            int32_t fd = events[i].data.fd;
            uint32_t evs = events[i].events;

            // Accept new clients
            if (fd == ssc->server)
            {
                for (;;)
                {
                    struct sockaddr_in client_addr = {0};
                    socklen_t client_len = sizeof(client_addr);

                    int32_t client_fd = accept(ssc->server, (struct sockaddr*)&client_addr, &client_len);
                    if (client_fd < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            break;
                        if (errno == EINTR || errno == ECONNABORTED)
                            continue;
                        if (errno == EMFILE || errno == ENFILE)
                        {
                            perror("accept fd limit");
                            break;
                        }
                        perror("accept failed");
                        break;
                    }

                    if (_set_nonblocking(client_fd) < 0)
                    {
                        perror("fcntl O_NONBLOCK");
                        close(client_fd);
                        continue;
                    }
                    if (fcntl(client_fd, F_SETFD, FD_CLOEXEC) < 0)
                    {
                        perror("fcntl FD_CLOEXEC");
                        close(client_fd);
                        continue;
                    }

                    struct epoll_event client_ev = {0};
                    client_ev.events = EPOLLIN;
                    client_ev.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) < 0)
                    {
                        perror("epoll_ctl ADD client failed");
                        close(client_fd);
                        continue;
                    }

                    if (ssc->verbose)
                    {
                        printf("New client connected: fd=%d\n", client_fd);
                    }
                }
                continue;
            }

            // Handle client errors/hangups
            if (evs & (EPOLLERR | EPOLLHUP))
            {
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                if (ssc->verbose)
                    printf("Client error/hangup: fd=%d\n", fd);
                continue;
            }

            if (fd < 0)
                continue;

            char tmp;
            ssize_t n = recv(fd, &tmp, 1, MSG_PEEK | MSG_DONTWAIT);
            if (n > 0)
            {
                job_t *job = create_job(respond, fd);
                if (job)
                    push_job(job, pool);
            }
            else if (n == 0)
            {
                // client closed
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
            }
        }
    }

    close(epoll_fd);
}

void close_server_socket_conn()
{
    if (!ssc)
        return;

    if (ssc->verbose)
	{
        puts("\nClosing server...");
    }

    if (epoll_fd >= 0)
	{
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ssc->server, NULL);
        close(epoll_fd);
        epoll_fd = -1;
    }

    close(ssc->server);
    free(ssc);
    ssc = NULL;
}
