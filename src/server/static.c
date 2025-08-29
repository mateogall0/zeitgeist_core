#include "server/static.h"
#include <stdbool.h>


// private to this file
static bool log_requests = true;


bool get_log_requests() {
    return log_requests;
}

void set_log_requests(bool value) {
    log_requests = value;
}
