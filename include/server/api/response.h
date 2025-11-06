#ifndef ZSERVER_API_RESPONSE_H
#define ZSERVER_API_RESPONSE_H
#include "server/api/endpoint.h"
#include <stdint.h>


typedef enum {
    METHOD_LINE = 0,
} line_idxs;


#define COLOR_RESET   "\033[0m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"


#define PRINT_METHOD_TARGET(method, target, status)         \
    do {                                                    \
        const char *m = (method) ? (method) : "NO_METHOD";  \
        const char *t = (target) ? (target) : "/";          \
        const char *status_color =                          \
            ((status) >= 500) ? COLOR_RED :                 \
            ((status) >= 400) ? COLOR_YELLOW :              \
            ((status) >= 300) ? COLOR_CYAN :                \
            COLOR_GREEN;                                    \
        printf("%s%s%s %s%s%s %s%d%s\n",                    \
               COLOR_CYAN, m, COLOR_RESET,                  \
               COLOR_MAGENTA, t, COLOR_RESET,               \
               status_color, (status), COLOR_RESET);        \
    } while (0)


void respond(int32_t client_fd);
void free_request(request_t *req);
int32_t print_request(request_t *req);
size_t
send_unrequested_payload(int sockfd,
                         char *buf,
                         size_t size);


#endif
