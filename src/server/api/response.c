#include "server/api/response.h"
#include "server/api/endpoint.h"
#include "server/api/socket.h"
#include "server/api/errors.h"
#include "server/api/headers.h"
#include "common/str.h"
#include "common/status.h"
#include "debug.h"
#include "server/static.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>


request_t *_parse_request(char *buff) {
    char *line, *key, *value;
    uint8_t i = 0;
    request_t *req = (request_t *)malloc(sizeof(request_t));

    if (!req)
        return (NULL);


    req->method = NULL;
    req->target = NULL;
    req->headers = create_headers_list();
    req->id = 0;

    req->body = sstrdup(cut_after_first_delim(buff, "\r\n\r\n"));

    line = strtok(buff, "\r\n");
    while(line) {
        key = sstrdup(line);
        if (i == METHOD_LINE) {
            value = cut_after_first_delim(key, " ");
            char *req_id_str = cut_after_first_delim(value, " ");
            if (!string_in_array(key, methods_str, METHODS_COUNT)) {
                free_request(req);
                return (NULL);
            }
            req->method = sstrdup(key);
            req->target = sstrdup(value);
            if (req_id_str)
                req->id = strtoul(req_id_str, NULL, 10);
        }
        else {
            request_header_t *new_header = create_push_header_to_list(req->headers,
                                                                      NULL,
                                                                      NULL);
            if (new_header) {
                new_header->value = sstrdup(value);
                new_header->key = sstrdup(key);
            }
        }
        free(key);
        i++;
        line = strtok(NULL, "\r\n");
    }
    return (req);
}


size_t
_send_response(int sockfd,
               char *buf,
               size_t size,
               char *method,
               char *target) {
    if (!buf)
        return (0);

    if (get_log_requests()) {
        printf("\033[36m%s\033[0m"
               " %s\n",
               method ? method : "no method",
               target ? target : "no target");
    }

    return (send(sockfd, buf, size, 0));
}

void respond(int32_t client_fd) {
    char *res = NULL, *buffer = NULL;
    char chunk[ZBUFF_CHUNK_SIZE];
    char *msg = NULL;
    request_t *req;
    methods m;
    endpoint_t *e;
    ssize_t n;
    size_t total = 0;
    char *(*h)(request_t *);
    found_endpoint_t *fe = NULL;

    print_debug("Reached Respond\n");

    while ((n = recv(client_fd, chunk, sizeof(chunk) - 1, 0)) > 0) {
        chunk[n] = '\0';
        if (!buffer) {
            buffer = malloc(n + 1);
            memcpy(buffer, chunk, n + 1);
            total = n;
        } else {
            buffer = realloc(buffer, total + n + 1);
            memcpy(buffer + total, chunk, n + 1);
            total += n;
        }
    }

    if (!buffer) {
        close(client_fd);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        return;
    }

    print_debug("received buffer in response: \n%s\n", buffer);

    req = _parse_request(buffer);
    free(buffer);
    print_debug("just tried to parse and freed the buffer\n");

    if (!req)
        goto fail_400;
    print_debug("successfully parsed the request\n");

    req->client_fd = client_fd;

    m = string_to_method(req->method);
    if (m >= METHODS_COUNT)
        goto fail_405;

    fe = find_endpoint(m, req->target);
    if (!fe)
        goto fail_400;
    if (!fe->found_target)
        goto fail_404;
    e = fe->endpoint;
    if (!e)
        goto fail_405;
    req->endpoint = e;

    res = e->handler(req);
    if (e->inmediate_res)
        goto cleanup;
    print_debug("%lu : response below\n", pthread_self());
    print_debug("%lu : %s\n", pthread_self(), res);
    print_debug("%lu : about to send response\n", pthread_self());
    _send_response(client_fd,
                   res,
                   strlen(res),
                   req->method,
                   req->target);
    print_debug("%lu : just sent response\n", pthread_self());

    goto cleanup;

fail_400:
    h = get_request_error_handler(RES_STATUS_BAD_REQUEST);
    goto send_failure;

fail_404:
    h = get_request_error_handler(RES_STATUS_NOT_FOUND);
    goto send_failure;

fail_405:
    h = get_request_error_handler(RES_STATUS_METHOD_NOT_ALLOWED);
    goto send_failure;

send_failure:
    if (!h)
        goto cleanup;
    msg = h(req);
    print_debug("About to send the following error msg:\n%s\n", msg);
    _send_response(client_fd,
                   msg,
                   strlen(msg),
                   req ? req->method : NULL,
                   req ? req->target : NULL);
    print_debug("sent the answer");

cleanup:
    if (fe)
        free(fe);
    if (msg)
        free(msg);
    if (res)
        free(res);
    free_request(req);
    print_debug("%lu : finished response process\n", pthread_self());
}

size_t
send_unrequested_payload(int sockfd,
                         char *buf,
                         size_t size) {
    if (!buf)
        return (0);
    return (send(sockfd, buf, size, 0));
}

void free_request(request_t *req) {
    if (!req)
        return;
    if (req->method)
        free(req->method);
    if (req->target)
        free(req->target);
    destroy_headers_list(req->headers);
    if (req->body)
        free(req->body);
    free(req);
}
