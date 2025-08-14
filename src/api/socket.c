#include "api/socket.h"
#include "api/response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/epoll.h>


volatile sig_atomic_t stop_loop = 0;

server_socket_conn_t *init_server_socket_conn(uint32_t port, uint8_t verbose)
{
    int32_t server;
    struct sockaddr_in server_addr, client_addr = {0};
    socklen_t client_addr_len = sizeof(client_addr);
    server_socket_conn_t *ssc = (server_socket_conn_t *)malloc(sizeof(server_socket_conn_t));


	server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1)
    {
        perror("Socket creation failed");
        return (NULL);
    }

	server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server, (struct sockaddr *)&server_addr,
        sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        return (NULL);
    }

    if (listen(server, MAX_SOCKET_CONNECTIONS) < 0) {
        perror("Listen failed");
        return (NULL);
    }
	ssc->port = port;
	ssc->verbose = verbose;
	ssc->server = server;
	ssc->client_addr_len = client_addr_len;
	ssc->client_addr = client_addr;
	if (verbose)
		printf("Server now running at %d\n", port);
	return (ssc);
}

void _handle_sigint(int sig) {
    (void)sig; // unused
    stop_loop = 1;
}

int32_t _set_nonblocking(int32_t fd) {
    int32_t flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void server_loop(server_socket_conn_t *ssc)
{
    stop_loop = 0;
    signal(SIGINT, _handle_sigint);

    int32_t epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("Epoll_create1 failed");
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = ssc->server;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ssc->server, &ev);

    _set_nonblocking(ssc->server);

    struct epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];

    while (!stop_loop)
	{
		int32_t n = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
		for (int32_t i = 0; i < n; i++)
		{
			if (events[i].data.fd == ssc->server)
			{
				// Stablish connection
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int32_t client_fd = accept(ssc->server, (struct sockaddr*)&client_addr, &client_len);
				if (client_fd >= 0)
				{
					_set_nonblocking(client_fd);
					struct epoll_event client_ev;
					client_ev.events = EPOLLIN;
					client_ev.data.fd = client_fd;
					epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
					if (ssc->verbose)
						printf("New client connected: fd=%d\n", client_fd);
				}
			}
			else
			{
				// Read from client input
				int32_t client_fd = events[i].data.fd;
				int32_t bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
				if (bytes <= 0)
				{
					// Connection closed or error
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
					close(client_fd);
					if (ssc->verbose)
						printf("Client disconnected: fd=%d\n", client_fd);
				}
				else
				{
					// Null-terminate
					buffer[bytes] = '\0';
					respond(client_fd, buffer);
				}
			}
		}
    }

    close(epoll_fd);
}

void close_server_socket_conn(server_socket_conn_t *ssc)
{
	if (ssc->verbose)
		puts("\nClosing server...");
	close(ssc->server);
	free(ssc);
}
