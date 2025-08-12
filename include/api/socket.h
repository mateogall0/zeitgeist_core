#ifndef SOCKET_H
#define SOCKET_H


#ifndef MAX_SOCKET_CONNECTIONS
#define MAX_SOCKET_CONNECTIONS 8192
#endif


#include <arpa/inet.h>
#include <stdint.h>


typedef struct {
	int32_t server;
	socklen_t client_addr_len;
	uint8_t verbose;
	uint32_t port;
} server_socket_conn_t;


server_socket_conn_t *init_server_socket_conn(uint32_t port, uint8_t verbose);
void server_socket_conn_loop(server_socket_conn_t *ssc);
void close_server_socket_conn(server_socket_conn_t *ssc);
#endif
