#include "client/master.h"
#include <stdlib.h>
#include <time.h>
#include "core/include/debug.h"


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

    connection = init_conn(NULL);
    if (!connection)
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
    destroy_conn(connection);
    free_unresolved_requests_list(unresolved_requests);
    destroy_client_payload_queue(unresolved_payload);
    destroy_client_payload_queue(resolved_payload);
    if (new)
        free(new);
    return (NULL);
}

int8_t
connect_zclient(zclient_handler_t *zclient,
                char *url, int32_t port) {
    if (!zclient)
        return (1);

    return (connect_client(zclient->connection, url, port));
}

unsigned long
_create_id(zclient_handler_t *zclient) {
    unsigned long id;
    do {
        id = ((unsigned long)rand() << 32) | rand();
    } while(id == 0 ||
            find_unresolved_request_from_list(zclient->unresolved_requests,
                                               id));
    return (id);
}

unsigned long  // returns the id of the request
zclient_make_request(zclient_handler_t *zclient,
                     methods method,
                     char *target,
                     char *headers,
                     char *body) {
    if (!zclient)
        return (0);

    unsigned long id = _create_id(zclient);
    ssize_t sent_bytes = send_request_payload(zclient->connection,
                                              method,
                                              target,
                                              headers,
                                              id,
                                              body);
    if (!sent_bytes)
        return (0);

    push_unresolved_request_from_list(zclient->unresolved_requests,
                                      id);

    return (id);
}

size_t
zclient_listen_input(zclient_handler_t *zclient) {
    if (!zclient ||
        !zclient->connection->connected)
        return (0);
    raw_received_payload_t *raw;

    raw = client_conn_recv(zclient->connection);
    if (!raw)
        return (0);

    print_debug("Raw received data:\n%s\n", raw->data);

    size_t len = raw->len;
    received_payload_t *rec = push_client_payload(zclient->unresolved_payload,
                                                  raw->data,
                                                  len);
    free(raw->data);
    free(raw);

    if (!rec)
        return (0);

    return (len);
}

void
zclient_process_input(zclient_handler_t *zclient) {
    if (!zclient)
        return;

    int processed_any = 0;
    received_payload_t *payload = NULL;

    while ((payload = pop_client_payload(zclient->unresolved_payload)) != NULL) {
        processed_any = 1;

        zclient_response_parsed_t *parsed = parse_response(payload->data);
        if (!parsed) {
            free_received_payload(payload);
            continue;
        }

        if (!parsed->formatted) {

            push_client_payload(zclient->resolved_payload,
                                payload->data,
                                payload->len);
        } else {
            unresolved_request_t *un =
                find_unresolved_request_from_list(zclient->unresolved_requests,
                                                  parsed->res->id);

            if (un)
                un->res = parsed->res;
            else
                free_response_parsed(parsed->res);
        }

        free_received_payload(payload);
        free(parsed);
    }

}

zclient_response_t *
zclient_get_response(zclient_handler_t *zclient,
                     unsigned long req_id) {
    if (!zclient)
        return (NULL);
    unresolved_request_t *un;
    un = find_unresolved_request_from_list(zclient->unresolved_requests,
                                           req_id);
    if (!un)
        return (NULL);

    zclient_response_t *res = un->res;
    if (!res)
        return (NULL);

    un = remove_unresolved_request_from_list(zclient->unresolved_requests,
                                             req_id);
    if (!un)
        return (NULL);
    free_unresolved_request(un);
    return (res);
}

received_payload_t *
zclient_pop_unrequested_payload(zclient_handler_t *zclient) {
    if (!zclient)
        return (NULL);
    return (pop_client_payload(zclient->resolved_payload));
}

void
free_zclient_response(zclient_response_t *res) {
    free_response_parsed(res);
}

void
disconnect_zclient(zclient_handler_t *zclient) {
    if (!zclient)
        return;
    disconnect(zclient->connection);
}

void
destroy_zclient(zclient_handler_t *zclient) {
    if (!zclient)
        return;
    disconnect_zclient(zclient);
    destroy_conn(zclient->connection);
    free_unresolved_requests_list(zclient->unresolved_requests);
    destroy_client_payload_queue(zclient->unresolved_payload);
    destroy_client_payload_queue(zclient->resolved_payload);
    free(zclient);
}
