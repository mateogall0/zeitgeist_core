#include "client/message.h"
#include <string.h>
#include "common/format.h"
#include "debug.h"
#include "common/str.h"

message_t *
parse_message(char *payload) {
    char *prefix = ZEIT_MESSAGE_SIGNATURE "\r\n";
    int prefix_len = strlen(prefix);

    if (strncmp(payload, prefix, prefix_len) != 0)
        goto fail;

    message_t *m = malloc(sizeof(message_t));
    if (!m)
        goto fail;

    char *payload_dup = strdup(&(payload[prefix_len]));
    if (!payload_dup)
        goto fail;
    print_debug("Dup cut payload: %s\n", payload_dup);


    m->headers = payload_dup;
    m->body = sstrdup(cut_after_first_delim(m->headers, "\r\n\r\n"));

    return (m);
fail:
    if (payload_dup)
        free(payload_dup);
    if (m)
        free(m);
    return (NULL);
}

void
free_message(message_t *msg) {
    if (!msg)
        return;
    if (msg->headers)
        free(msg->headers);
    if (msg->body)
        free(msg->body);
    free(msg);
}
