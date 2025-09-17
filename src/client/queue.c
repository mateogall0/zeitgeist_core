#include "client/queue.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "common/str.h"


received_payload_queue_t *
init_client_payload_queue(size_t capacity) {
    if (capacity == 0)
        return (NULL);
    received_payload_queue_t *new = malloc(sizeof(received_payload_queue_t));
    if (!new)
        return (NULL);

    new->capacity = capacity;
    new->size = 0;
    new->head = NULL;
    new->tail = NULL;

    return (new);
}

void
clear_client_payload_queue(received_payload_queue_t *q) {
    if (!q)
        return;

    while(q->size > 0) {
        received_payload_t *tmp = pop_client_payload(q);
        free_received_payload(tmp);
    }
}

received_payload_t *
pop_client_payload(received_payload_queue_t *q) {
    if (!q || q->size == 0)
        return (NULL);

    received_payload_t *res = q->head;
    if (q->size == 1) {
        q->head = NULL;
        q->tail = NULL;
    }

    q->head = res->next;
    --q->size;
    return (res);
}

received_payload_t *
push_client_payload(received_payload_queue_t *q, char *data, size_t len) {
    if (!q ||
        q->size == q->capacity ||
        !data ||
        len < 1)
        return (NULL);

    received_payload_t *new = (received_payload_t *)malloc(sizeof(received_payload_t));

    if (!new)
        return (NULL);

    new->next = NULL;
    new->data = strdup(data);
    new->len = len;

    new->next = q->head;
    q->head = new;
    ++q->size;

    return (new);
}

void
destroy_client_payload_queue(received_payload_queue_t *q) {
    if (!q)
        return;
    clear_client_payload_queue(q);
    free(q);
}

void
free_received_payload(received_payload_t *p) {
    free(p->data);
    free(p);
}

int
print_received_payload(received_payload_t *p) {
    if (!p)
        return (0);

    return (printf("%5s: %p\n%5s: %ld\n%5s: %s",
                   "Id", &p,
                   "Len", p->len,
                   "Data", p->data));
}

int
pall_client_received_payload(received_payload_queue_t *q) {
    if (!q)
        return (-1);

    int32_t count = 0;
    received_payload_t *current = q->head;

    while(current) {
        count += print_received_payload(current);
        current = current->next;
    }
    return (count);
}
