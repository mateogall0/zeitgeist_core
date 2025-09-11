#ifndef ZSERVER_API_RESPONSE_H
#define ZSERVER_API_RESPONSE_H
#include "server/api/endpoint.h"
#include <stdint.h>


typedef enum {
    METHOD_LINE = 0,
} line_idxs;


void respond(int32_t client_fd);
void free_request(request_t *req);
int32_t print_request(request_t *req);
size_t send_unrequested_payload(int sockfd, char *buf, size_t size);


#endif
