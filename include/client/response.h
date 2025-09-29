#ifndef ZCLIENT_RESPONSE_H
#define ZCLIENT_RESPONSE_H


#include <stdbool.h>


typedef struct zclient_response_s {
    unsigned long id;
    char *headers;
    char *body;
    char *status_msg;
} zclient_response_t;


typedef struct {
    zclient_response_t *res;
    bool formatted;
} zclient_response_parsed_t;


zclient_response_parsed_t *
parse_response(char *payload);

void
free_response_parsed(zclient_response_t *res);

#endif
