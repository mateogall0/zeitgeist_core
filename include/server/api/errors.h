#ifndef ZSERVER_API_ERRORS_H
#define ZSERVER_API_ERRORS_H

#include "server/api/response.h"
#include "common/status.h"
#include <stdint.h>


typedef struct request_error_s{
    struct request_error_s *next;
    status_code_e status;
    char *(*handler)(request_t *);
} request_error_t;

typedef struct {
    request_error_t *head;
} request_errors_list_t;

extern request_errors_list_t *request_errors;

request_errors_list_t *init_request_errors_list();
request_error_t *set_request_error(status_code_e status, char *(*handler)(request_t *));
char *(*get_request_error_handler(status_code_e status))(request_t *);
void destroy_request_errors_list();


#endif
