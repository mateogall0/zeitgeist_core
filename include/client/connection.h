#ifndef ZCONNECTION_H
#define ZCONNECTION_H


#include "common/methods.h"
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/types.h>


#ifndef CLIENT_BUFFER_SIZE
#define CLIENT_BUFFER_SIZE 8192
#endif


typedef struct {
    int32_t client;
    struct sockaddr_in server_addr;
    int8_t connected;
    void(*process)(char *, size_t);
} connection_t;

typedef struct {
    char *data;
    size_t len;
} raw_received_payload_t;

connection_t *init_conn(void(*process)(char *, size_t));
int8_t connect_client(connection_t *c, char *url, int32_t port);
ssize_t send_payload(connection_t *c, char *payload, size_t len);
raw_received_payload_t *client_conn_recv(connection_t *c);
void client_conn_loop(connection_t *c);
int8_t disconnect(connection_t *c);

ssize_t
send_request_payload(connection_t *c,
                     methods method,
                     char *target,
                     char *headers,
                     unsigned long req_id,
                     char *body);

void destroy_conn(connection_t *conn);

#endif
