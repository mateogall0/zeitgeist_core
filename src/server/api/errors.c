#include "server/api/errors.h"
#include "server/api/response.h"
#include <stdlib.h>
#include <stdint.h>


request_errors_list_t *request_errors = NULL;


request_errors_list_t *init_request_errors_list() {
    if (request_errors)
        return (NULL);

    request_errors = (request_errors_list_t *)malloc(sizeof(request_errors_list_t));
    if (!request_errors)
        return (NULL);

    request_errors->head = NULL;

    return (request_errors);
}

request_error_t *set_request_error(status_code_e status, char *(*handler)(request_t *)) {
    if (!request_errors || get_request_error_handler(status) || !handler)
        return (NULL);

    request_error_t *request_error = (request_error_t *)malloc(sizeof(request_error_t));
    if (!request_error)
        return (NULL);

    request_error->next = request_errors->head;
    request_error->status = status;
    request_error->handler = handler;
    request_errors->head = request_error;

    return (request_error);
}

char *(*get_request_error_handler(status_code_e status))(request_t *) {
    if (!request_errors)
        return (NULL);

    request_error_t *current = request_errors->head;

    while(current) {
        if (current->status == status)
            return (current->handler);
        current = current->next;
    }

    return (NULL);

}

void destroy_request_errors_list() {
    if (!request_errors)
        return;
    request_error_t *current = request_errors->head, *next;

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }

    free(request_errors);
    request_errors = NULL;
}
