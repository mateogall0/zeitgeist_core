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

const char *methods_str[METHODS_COUNT] = {
    "GET",
    "PUT",
    "DELETE",
    "POST"
};

typedef struct {
	char *content_type;
	char *headers;
	char *body;
} request_t;

typedef struct endpoint_s{
	methods method;
	char *target;
	char (*handler)(request_t *);
	struct endpoint_s *next;
} endpoint_t;

typedef struct {
	endpoint_t *head;
} endpoint_list_t;

endpoint_list_t *endpoints = NULL;

endpoint_list_t *init_endpoints_list();
endpoint_t *set_endpoint(char *method, char *target, char (*handler)(request_t *));
int32_t print_endpoint(endpoint_t e);
int32_t pall_endpoints();
void destroy_endpoints();

#endif
