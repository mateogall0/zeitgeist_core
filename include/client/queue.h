#ifndef ZCLIENT_QUEUE_H
#define ZCLIENT_QUEUE_H


#include <stddef.h>
#include <stdint.h>


typedef struct received_payload_s
{
    struct received_payload_s *next;
    size_t len;
    char *data;
} received_payload_t;

typedef struct
{
    received_payload_t *head;
    received_payload_t *tail;
    size_t capacity;
    size_t size;
} received_payload_queue_t;


received_payload_queue_t *
init_client_payload_queue(size_t capacity);

void
clear_client_payload_queue(received_payload_queue_t *q);

received_payload_t *
pop_client_payload(received_payload_queue_t *q);

received_payload_t *
push_client_payload(received_payload_queue_t *q, char *data, size_t len);

void
destroy_client_payload_queue(received_payload_queue_t *q);

void
free_received_payload(received_payload_t *p);

int
print_received_payload(received_payload_t *p);

int
pall_client_received_payload(received_payload_queue_t *q);


#endif
