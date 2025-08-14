#ifndef RESPONSE_H
#define RESPONSE_H
#include "api/endpoint.h"
#include <stdint.h>


typedef enum {
	METHOD = 0,
	CONTENT_TYPE = 1,
	HEADERS = 2,
} line_idxs;


request_t *_parse_request(char *buff);
void respond(int32_t client_fd, char *buffer);
void free_request(request_t *req);
int32_t print_request(request_t *req);


#endif
