#include "common/methods.h"
#include "server/api/endpoint.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


const char *methods_str[METHODS_COUNT] = {
    "GET",
    "PUT",
    "DELETE",
    "POST"
};

endpoint_list_t *endpoints = NULL;

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
