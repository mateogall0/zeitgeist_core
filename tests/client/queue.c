#include "tests.h"
#include "zeitgeist_client.h"
#include "fixtures.h"
#include <string.h>


int8_t test_client_queue_creation() {
    received_payload_queue_t *q = init_client_payload_queue(5);
    ASSERT(q);
    ASSERT(q->size == 0);
    ASSERT(q->capacity == 5);
    ASSERT(!q->head);
    push_client_payload(q, "Hello\n", 6);
    ASSERT(q->size == 1);
    destroy_client_payload_queue(q);

    return (0);
}

int8_t test_client_queue_capacity() {
    size_t capacity = 5;
    received_payload_queue_t *q = init_client_payload_queue(capacity);
    for (size_t i = 0; i < capacity; i++) {
        push_client_payload(q, "Hello\n", 6);
        ASSERT(q->size == i + 1);
    }
    for (size_t i = 0; i < capacity * 2; i++) {
        push_client_payload(q, "Hello\n", 6);
        ASSERT(q->size == capacity);
    }

    destroy_client_payload_queue(q);

    return (0);
}

int8_t test_client_queue_pop() {
    size_t capacity = 5;
    received_payload_queue_t *q = init_client_payload_queue(capacity);

    ASSERT(pop_client_payload(q) == NULL);

    push_client_payload(q, "Hello\n", 6);
    ASSERT(q->size == 1);

    received_payload_t *popped = pop_client_payload(q);

    ASSERT(popped);
    ASSERT(!popped->next);
    ASSERT(strcmp(popped->data, "Hello\n") == 0);
    ASSERT(popped->len == 6);
    ASSERT(q->size == 0);

    free_received_payload(popped);

    destroy_client_payload_queue(q);

    return (0);
}

int8_t test_client_queue_pop_all() {
    size_t capacity = 5, i;
    char *data[] = {
        "Hello",
        "Hello again",
        "This is getting weird",
        "Why?",
        "Sorry, this is unprofessional"
    };
    received_payload_queue_t *q = init_client_payload_queue(capacity);

    ASSERT(pop_client_payload(q) == NULL);

    for (i = 0; i < capacity; i++) {
        push_client_payload(q, data[i], strlen(data[i]));
        ASSERT(q->size == i + 1);
    }
    for (; i-- > 0; ) {
        received_payload_t *rp = pop_client_payload(q);
        ASSERT(rp);
        ASSERT(strcmp(rp->data, data[i]) == 0);
        free_received_payload(rp);
    }

    destroy_client_payload_queue(q);

    return (0);
}
