#include "server/api/endpoint.h"
#include "common/str.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>


const char *methods_str[METHODS_COUNT] = {
    "GET",
    "PUT",
    "DELETE",
    "POST"
};

endpoint_list_t *endpoints = NULL;

endpoint_list_t *init_endpoints_list() {
    if (endpoints) {
        perror("Endpoints already initialized");
        return (endpoints);
    }
    endpoints = (endpoint_list_t *)malloc(sizeof(endpoint_list_t));
    endpoints->head = NULL;
    return (endpoints);
}

endpoint_t *
_set_endpoint(methods method,
              char *target,
              char *(*handler)(request_t *),
              bool inmediate_res,
              bool mock_http) {
    endpoint_t *endpoint;

    if (!endpoints ||!target || method >= METHODS_COUNT ||
        method < 0 || !handler)
        return (NULL);

    endpoint = (endpoint_t *)malloc(sizeof(endpoint_t));
    if (!endpoint)
        return (NULL);
    endpoint->method = method;
    endpoint->target = strdup(target);
    endpoint->handler = handler;
    endpoint->next = endpoints->head;
    endpoint->mock_http = mock_http;
    endpoint->inmediate_res = inmediate_res;

    endpoints->head = endpoint;
    return (endpoint);
}

endpoint_t *
set_endpoint_va(int8_t ac,
                 ...) {
    if (ac < 3)
        return (NULL);

    methods method;
    char *target;
    char *(*handler)(request_t *);
    bool mock_http = false;
    bool inmediate_res = false;
    va_list ap;
    va_start(ap, ac);
    for (int i = 0; i < ac; i++) {
        switch (i) {
        case 0:
            method = va_arg(ap, methods);
            break;
        case 1:
            target = va_arg(ap, char *);
            break;
        case 2:
            handler = va_arg(ap, char* (*)(request_t *));
            break;
        case 3:
            inmediate_res = (bool)va_arg(ap, int);
            break;
        case 4:
            mock_http = (bool)va_arg(ap, int);
            break;
        }
    }
    va_end(ap);

    return(_set_endpoint(method,
                         target,
                         handler,
                         inmediate_res,
                         mock_http));
}

endpoint_t *
set_endpoint(methods method,
             char *target,
             char *(*handler)(request_t *)) {
    return (set_endpoint_va(3, method, target, handler));
}

int32_t print_endpoint(endpoint_t *e) {
    if (!e)
        return (-1);

    return (printf("%s %s\n", methods_str[e->method], e->target));
}

int32_t pall_endpoints() {
    int32_t count = 0;
    endpoint_t *current;

    if (!endpoints)
        return (-1);
    current = endpoints->head;
    while(current) {
        count += print_endpoint(current);
        current = current->next;
    }
    return (count);
}

void destroy_endpoints() {
    if (!endpoints)
        return;
    endpoint_t *current;
    while(endpoints->head)
    {
        current = endpoints->head;
        endpoints->head = endpoints->head->next;
        free(current->target);
        free(current);
    }
    free(endpoints);
    endpoints = NULL;
}

found_endpoint_t *find_endpoint(methods method, char *target) {
    if (!endpoints || method < 0 || method >= METHODS_COUNT || !target)
        return (NULL);

    found_endpoint_t *fe = malloc(sizeof(found_endpoint_t));
    if (!fe)
        return (NULL);
    fe->endpoint = NULL;
    fe->found_target = false;

    endpoint_t *current = endpoints->head;

    while (current) {
        if (strcmp(target, current->target) == 0) {
            fe->found_target = true;
            if (current->method == method) {
                fe->endpoint = current;
                break;
            }
        }
        current = current->next;
    }
    return (fe);
}

methods string_to_method(const char *str) {
    int32_t i;

    if(!str)
        return (METHODS_COUNT);

    for (i = 0; i < METHODS_COUNT; i++) {
        if (strcmp(str, methods_str[i]) == 0)
            return ((methods)i);
    }
    return (METHODS_COUNT);
}
