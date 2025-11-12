#include "client/response.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common/str.h"
#include "common/format.h"


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

    char *payload_dup = strdup(payload);
    if (!payload_dup)
        goto fail;
    bool formatted = true;

    res->headers = sstrdup(cut_after_first_delim(payload_dup , "\r\n"));
    res->body = sstrdup(cut_after_first_delim(res->headers, "\r\n\r\n"));

    int i = 0;
    char *tok = strtok(payload_dup , RESPONSE_TARGET_DELIM);
    while (tok) {
        if (!tok) {
            formatted = false;
            break;
        }
        switch (i) {
        case 0:
            if (strcmp(tok, ZEIT_RESPONSE_VERSION) != 0)
                formatted = false;
            break;
        case 1:
            res->status_msg = sstrdup(tok);
            break;
        case 2:
            res->id = strtoul(tok, NULL, 10);
            break;
        default:
            formatted = false;
            break;
        }
        tok = strtok(NULL, RESPONSE_TARGET_DELIM);
        ++i;
    }

    parsed->res = res;
    parsed->formatted = formatted;

    free(payload_dup);

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

    if (res->headers)
        free(res->headers);
    if (res->body)
        free(res->body);
    if (res->status_msg)
        free(res->status_msg);

    free(res);
}
