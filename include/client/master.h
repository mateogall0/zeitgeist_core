#ifndef ZCLIENT_MASTER_H
#define ZCLIENT_MASTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "client/response.h"
#include "client/unresolved.h"
#include "client/connection.h"
#include "client/queue.h"
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

zclient_handler_t *
create_zclient(size_t unresolved_payload_capacity,
               size_t resolved_payload_capacity);

int8_t
connect_zclient(zclient_handler_t *zclient,
                char *url, int32_t port);

unsigned long
zclient_make_request(zclient_handler_t *zclient,
                     methods method,
                     char *target,
                     char *headers,
                     char *body);

size_t
zclient_listen_input(zclient_handler_t *zclient);

void
zclient_process_input(zclient_handler_t *zclient);

zclient_response_t *
zclient_get_response(zclient_handler_t *zclient,
                     unsigned long req_id);

received_payload_t *
zclient_pop_unrequested_payload(zclient_handler_t *zclient);

void
free_zclient_response(zclient_response_t *res);

void
disconnect_zclient(zclient_handler_t *zclient);

void
destroy_zclient(zclient_handler_t *zclient);

#ifdef __cplusplus
}
#endif

#endif
