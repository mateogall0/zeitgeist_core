#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "common/methods.h"
#include "server/api/headers.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


typedef struct request_s request_t;

typedef struct endpoint_s{
    methods method;
    char *target;
    char *(*handler)(request_t *);
    struct endpoint_s *next;
    bool mock_http;
    bool inmediate_res;
    uintptr_t meta;
} endpoint_t;

typedef struct {
    endpoint_t *head;
} endpoint_list_t;

struct request_s {
    request_headers_list_t *headers;
    char *body;
    char *method;
    char *target;
    char *query_params;
    char **path_params;
    int client_fd;
    unsigned long id;
    endpoint_t *endpoint;
};

typedef struct {
    endpoint_t *endpoint;
    bool found_target;
} found_endpoint_t;

extern endpoint_list_t *endpoints;


endpoint_list_t *init_endpoints_list();
endpoint_t *set_endpoint_va(int8_t ac, ...);
endpoint_t *set_endpoint(methods method, char *target, char *(*handler)(request_t *));
int32_t print_endpoint(endpoint_t *e);
int32_t pall_endpoints();
void destroy_endpoints();
found_endpoint_t *find_endpoint(methods method, char *target);

#endif
