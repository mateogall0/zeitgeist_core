#ifndef ZCLIENT_MASTER_H
#define ZCLIENT_MASTER_H


#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>
#include "client/connection.h"
#include "client/queue.h"
#include "client/unresolved.h"
#include "common/methods.h"


typedef struct {
    connection_t *connection;
    received_payload_queue_t *unresolved_payload;
    received_payload_queue_t *resolved_payload;
    unresolved_requests_list_t *unresolved_requests;
} zclient_handler_t;

typedef struct {
    char *data;
    size_t len;
} zclient_unrequested_payload_t;

typedef struct {
    unsigned int id;
    methods method;
    char *target;
    char *headers;
    char *body;
} zclient_response_t;

zclient_handler_t *
create_zclient(size_t unresolved_payload_capacity,
               size_t resolved_payload_capacity);

bool
connect_zclient(zclient_handler_t *zclient,
                char *url, int32_t port);

unsigned long
zclient_make_request(zclient_handler_t *zclient,
                     methods method,
                     char *target,
                     char *headers,
                     char *body);

zclient_response_t *
zclient_read_response(zclient_handler_t *zclient,
                      unsigned long req_id);

zclient_response_t *
zclient_pop_unrequested_payload(zclient_handler_t *zclient);

void
free_zclient_response(zclient_response_t *res);

void
disconnect_zclient(zclient_handler_t *zclient);

void
destroy_zclient(zclient_handler_t *zclient);

#endif
