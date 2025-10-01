#include "tests.h"
#include "client/master.h"
#include "common/format.h"
#include "server/api/endpoint.h"
#include "server/api/socket.h"
#include "server/api/endpoint.h"
#include "server/api/response.h"
#include "server/api/errors.h"
#include "server/sessions/wheel.h"
#include "server/sessions/map.h"
#include "common/status.h"
#include "utils.h"
#include "fixtures.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <stdint.h>



char *_process_formatted_endpoint_example(request_t *req) {
    char *buf;
    asprintf(&buf,
             "%s 200 %lu\r\n"
             "Example: This is an example\r\n"
             "\r\n"
             "Hello from the body!",
             ZEIT_RESPONSE_VERSION,
             req->id);
    return (buf);
}

int8_t test_request_static_payload() {
    ASSERT(!endpoints);
    init_endpoints_list();
    ASSERT(endpoints);
    ASSERT(endpoints->head == NULL);
    endpoint_t *e = set_endpoint(GET,
                                 "/example",
                                 _process_formatted_endpoint_example);
    ASSERT(e);

    SETUP_SERVER_SOCKET();
    RUN_SERVER_SOCKET_LOOP();
    zclient_handler_t *z = create_zclient(1, 1);
    ASSERT(z);

    ASSERT(connect_zclient(z, "127.0.0.1", SERVER_PORT) == 0);

    ASSERT(!z->unresolved_requests->head);

    unsigned long req_id = zclient_make_request(z,
                                                GET,
                                                "/example",
                                                "Example: Another example",
                                                "");
    ASSERT(req_id > 0);
    ASSERT(z->unresolved_requests->head);
    ASSERT(zclient_listen_input(z) > 0);
    zclient_process_input(z);
    zclient_response_t *res = zclient_get_response(z, req_id);
    ASSERT(res);
    ASSERT(strcmp(res->headers, "Example: This is an example") == 0);
    ASSERT(strcmp(res->body, "Hello from the body!") == 0);
    ASSERT(strcmp(res->status_msg, "200") == 0);
    ASSERT(res->id == req_id);

    free_response_parsed(res);
    ASSERT(!z->unresolved_requests->head);

    destroy_zclient(z);

    DESTROY_SERVER_SOCKET();
    destroy_endpoints();
    ASSERT(!endpoints);
    return (0);
}
