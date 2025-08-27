#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    GET,
    PUT,
    DELETE,
    POST,
    METHODS_COUNT
} methods;

extern const char *methods_str[METHODS_COUNT];

typedef struct {
    char *content_type;
    char *headers;
    char *body;
    char *method;
    char *target;
} request_t;

typedef struct endpoint_s{
    methods method;
    char *target;
    char *(*handler)(request_t *);
    struct endpoint_s *next;
} endpoint_t;

typedef struct {
    endpoint_t *head;
} endpoint_list_t;

extern endpoint_list_t *endpoints;

endpoint_list_t *init_endpoints_list();
endpoint_t *set_endpoint(methods method, char *target, char *(*handler)(request_t *));
int32_t print_endpoint(endpoint_t *e);
int32_t pall_endpoints();
void destroy_endpoints();
endpoint_t *find_endpoint(methods method, char *target);
methods string_to_method(const char *str);

#endif
