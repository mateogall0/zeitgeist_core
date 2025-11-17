#ifndef ZCLIENT_MESSAGE_H
#define ZCLIENT_MESSAGE_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>

typedef struct {
    char *headers;
    char *body;
} message_t;

message_t *
parse_message(char *payload);

void
free_message(message_t *msg);

#ifdef __cplusplus
}
#endif

#endif
