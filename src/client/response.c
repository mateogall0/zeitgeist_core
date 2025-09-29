#include "client/response.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


zclient_response_parsed_t *
parse_response(char *payload) {
    zclient_response_parsed_t *parsed;
    zclient_response_t *res;

    if (!payload)
        goto fail;

    parsed = malloc(sizeof(zclient_response_parsed_t));
    if (!parsed)
        goto fail;
    res = malloc(sizeof(zclient_response_t));
    if (!res)
        goto fail;
    bool formatted = true;

    parsed->formatted = formatted;

    return (parsed);
fail:
    if (parsed)
        free(parsed);
    free_response_parsed(res);
    return (NULL);
}

void
free_response_parsed(zclient_response_t *res) {
    if (!res)
        return;
}
