#ifndef SOCKET_H
#define SOCKET_H


#ifndef MAX_SOCKET_CONNECTIONS
#define MAX_SOCKET_CONNECTIONS 8
#endif

#ifndef ZBUFF_CHUNK_SIZE
#define ZBUFF_CHUNK_SIZE 4096
#endif

#ifndef MAX_EVENTS
#define MAX_EVENTS 4096
#endif

//in miliseconds
#ifndef TIMEOUT
#define TIMEOUT 1000
#endif

#include "server/thread.h"

#include <arpa/inet.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct {
    int32_t server;
    socklen_t client_addr_len;
    bool verbose;
    uint32_t port;
    struct sockaddr_in client_addr;
#ifdef __APPLE__
    int kqueue_fd;
#endif
} server_socket_conn_t;


extern server_socket_conn_t *ssc;

void init_server_socket_conn(uint32_t port, bool verbose);
void server_loop(void (*handle_input)(int client_fd));
void close_server_socket_conn();

#endif
