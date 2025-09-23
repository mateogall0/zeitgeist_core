#include "client/master.h"
#include <stdlib.h>


zclient_handler_t *
create_zclient(size_t unresolved_payload_capacity,
               size_t resolved_payload_capacity) {
    zclient_handler_t *new = NULL;
    connection_t *connection = NULL;
    received_payload_queue_t *unresolved_payload = NULL;
    received_payload_queue_t *resolved_payload = NULL;
    unresolved_requests_list_t *unresolved_requests = NULL;

    if (unresolved_payload_capacity == 0 ||
        resolved_payload_capacity == 0)
        goto fail;

    new = malloc(sizeof(zclient_handler_t));
    if (!new)
        goto fail;

    unresolved_payload = init_client_payload_queue(unresolved_payload_capacity);
    if (!unresolved_payload)
        goto fail;

    resolved_payload = init_client_payload_queue(resolved_payload_capacity);
    if (!resolved_payload)
        goto fail;

    unresolved_requests = create_unresolved_requests_list();
    if (!unresolved_requests)
        goto fail;

    new->connection = connection;
    new->unresolved_payload = unresolved_payload;
    new->resolved_payload = resolved_payload;
    new->unresolved_requests = unresolved_requests;

    return (new);

fail:
    free_unresolved_requests_list(unresolved_requests);
    destroy_client_payload_queue(unresolved_payload);
    destroy_client_payload_queue(resolved_payload);
    if (new)
        free(new);
    return (NULL);
}

bool
connect_zclient(zclient_handler_t *zclient,
                char *url, int32_t port);

unsigned long  // returns the id of the request
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
