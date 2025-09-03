#ifndef ZSERVER_API_RESPONSE_H
#define ZSERVER_API_RESPONSE_H
#include "server/api/endpoint.h"
#include <stdint.h>


typedef enum {
    METHOD = 0,
    CONTENT_TYPE = 1,
    HEADERS = 2,
} line_idxs;


void respond(int32_t client_fd);
void free_request(request_t *req);
int32_t print_request(request_t *req);


#endif
