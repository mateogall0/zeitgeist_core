#ifndef ZCLIENT_MESSAGE_H
#define ZCLIENT_MESSAGE_H

#include <stdlib.h>

typedef struct {
    char *headers;
    char *body;
} message_t;

message_t *
parse_message(char *payload);

void
free_message(message_t *msg);

#endif
