#include "server/api/headers.h"
#include <string.h>
#include <stdlib.h>


request_headers_list_t *
create_headers_list() {
    request_headers_list_t *new = malloc(sizeof(request_headers_list_t));
    if (!new)
        return (NULL);
    new->head = NULL;
    return (new);
}

request_header_t *
create_push_header_to_list(request_headers_list_t *list,
                           char *key,
                           char *value) {
    if (!list || !key || !value)
        return (NULL);
    request_header_t *new = malloc(sizeof(request_header_t));
    if (!new)
        return (NULL);

    new->key = strdup(key);
    new->value = strdup(value);
    new->next = list->head;
    list->head = new;
    return (new);
}

void
destroy_headers_list(request_headers_list_t *list) {
    if (!list)
        return;

    request_header_t *current = list->head, *temp;
    while (current) {
        temp = current->next;
        free(current->key);
        free(current->value);
        free(current);
        current = temp;
    }
    free(list);
}
