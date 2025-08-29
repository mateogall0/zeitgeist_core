#include "client/queue.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "common/str.h"


received_payload_queue_t *client_payload_queue = NULL;

int8_t init_client_payload_queue(size_t capacity) {
    if (client_payload_queue)
        return (1);

    client_payload_queue = (received_payload_queue_t *)malloc(sizeof(received_payload_queue_t));
    if (!client_payload_queue)
        return (1);

    client_payload_queue->head = NULL;
    client_payload_queue->tail = NULL;
    client_payload_queue->capacity = capacity;
    client_payload_queue->size = 0;

    return (0);
}

void clear_client_payload_queue() {
    if (!client_payload_queue)
        return;

    received_payload_t *tmp;
    while (client_payload_queue->size > 0) {
        tmp = pop_client_payload();
        free_received_payload(tmp);
    }
}

received_payload_t *pop_client_payload() {
    if (!client_payload_queue || client_payload_queue->size == 0)
        return (NULL);

    received_payload_t *res = client_payload_queue->head;
    if (client_payload_queue->size == 1) {
        client_payload_queue->head = NULL;
        client_payload_queue->tail = NULL;
    }

    client_payload_queue->head = res->next;
    --client_payload_queue->size;
    return (res);
}

received_payload_t *push_client_payload(char *data, size_t len) {
    if (!client_payload_queue ||
        client_payload_queue->size == client_payload_queue->capacity ||
        !data ||
        len < 1)
        return (NULL);

    received_payload_t *new = (received_payload_t *)malloc(sizeof(received_payload_t));

    if (!new)
        return (NULL);

    new->next = NULL;
    new->data = strdup(data);
    new->len = len;

    new->next = client_payload_queue->head;
    client_payload_queue->head = new;
    ++client_payload_queue->size;

    return (new);
}

int8_t destroy_client_payload_queue() {
    if (!client_payload_queue)
        return (-1);
    clear_client_payload_queue();
    free(client_payload_queue);
    client_payload_queue = NULL;
    return (0);
}

void free_received_payload(received_payload_t *p) {
    free(p->data);
    free(p);
}

int32_t pall_client_received_payload()
{
    if (!client_payload_queue)
        return (-1);

    int32_t count = 0;
    received_payload_t *current = client_payload_queue->head;

    while(current) {
        count += print_received_payload(current);
        current = current->next;
    }
    return (count);
}

int32_t print_received_payload(received_payload_t *p) {
    if (!p)
        return (0);

    return (printf("%5s: %p\n%5s: %ld\n%5s: %s",
                   "Id", &p,
                   "Len", p->len,
                   "Data", p->data));
}
