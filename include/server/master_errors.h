#ifndef ZSERVER_MASTER_ERRORS_H
#define ZSERVER_MASTER_ERRORS_H


#include "server/api/response.h"
#include "server/api/errors.h"
#include "common/status.h"


typedef struct {
    status_code_e status;
    char *(*handler)(request_t *req);
} errors_tobe_initialize_t;


void initialize_static_errors();


#endif
