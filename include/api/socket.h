#ifndef SOCKET_H
#define SOCKET_H


#ifndef MAX_SOCKET_CONNECTIONS
#define MAX_SOCKET_CONNECTIONS 8
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 8192
#endif

#ifndef MAX_EVENTS
#define MAX_EVENTS 4096
#endif

//in miliseconds
#ifndef TIMEOUT
#define TIMEOUT 1000
#endif

#include "thread.h"

#include <arpa/inet.h>
#include <stdint.h>


typedef struct {
	int32_t server;
	socklen_t client_addr_len;
	uint8_t verbose;
	uint32_t port;
	struct sockaddr_in client_addr;
} server_socket_conn_t;


extern server_socket_conn_t *ssc;

void init_server_socket_conn(uint32_t port, uint8_t verbose);
void server_loop(thread_pool_t *pool);
void close_server_socket_conn();

#endif
