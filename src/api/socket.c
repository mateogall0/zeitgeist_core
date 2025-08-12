#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>



server_socket_conn_t *init_server_socket_conn(uint32_t port, uint8_t verbose)
{
	int32_t server;
	struct sockaddr_in server_addr, client_addr;
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
	if (verbose)
		printf("Server now running at %d\n", port);
	return (ssc);
}

void server_socket_conn_loop(server_socket_conn_t *ssc)
{
}

void close_server_socket_conn(server_socket_conn_t* ssc)
{
	close(ssc->server);
	free(ssc);
}
