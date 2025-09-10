#include "server/master_errors.h"
#include "common/status.h"
#include <stdio.h>
#include <string.h>


char *_400(request_t *r) {
    (void)r;
    char *msg = strdup("400 Bad request");
    return (msg);
}

char *_404(request_t *r) {
    (void)r;
    char *msg = strdup("404 Resource not found");
    return (msg);
}

char *_405(request_t *r) {
    (void)r;
    char *msg = strdup("405 Method not allowed");
    return (msg);
}

void initialize_static_errors() {
    if (!init_request_errors_list()) {
        fprintf(stderr, "Error initializing errors list\n");
        return;
    }

    errors_tobe_initialize_t tobe_init[] = {
        {RES_STATUS_BAD_REQUEST, _400},
        {RES_STATUS_NOT_FOUND, _404},
        {RES_STATUS_METHOD_NOT_ALLOWED, _405},
        {0, NULL}
    };
    for (uint16_t i = 0; i[tobe_init].handler; ++i) {
        if (!set_request_error(i[tobe_init].status,
                               i[tobe_init].handler)) {
            fprintf(stderr, "Failed creating error for %d\n",
                    i[tobe_init].status);
            break;
        }
    }
}
