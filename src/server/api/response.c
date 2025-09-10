#include "server/api/response.h"
#include "server/api/endpoint.h"
#include "server/api/socket.h"
#include "server/api/errors.h"
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
    req->content_type = NULL;
    req->headers = NULL;

    req->body = sstrdup(cut_after_first_delim(buff, "\r\n\r\n"));

    line = strtok(buff, "\r\n");
    while(line) {
        key = sstrdup(line);
        switch (i) {
        case METHOD:
            value = cut_after_first_delim(key, " ");
            if (!string_in_array(key, methods_str, METHODS_COUNT)) {
                free_request(req);
                return (NULL);
            }
            req->method = sstrdup(key);
            req->target = sstrdup(value);
            break;
        case CONTENT_TYPE:
            value = cut_after_first_delim(key, ": ");
            if (strcmp(key, "Content-Type") != 0) {
                free_request(req);
                free(key);
                return (NULL);
            }
            req->content_type = sstrdup(value);
            break;
        case HEADERS:
            value = cut_after_first_delim(key, ": ");
            if (strcmp(key, "Headers") != 0) {
                free_request(req);
                free(key);
                return (NULL);
            }
            req->headers = sstrdup(value);
            break;
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
               method,
               target);
    }

    return (send(sockfd, buf, size, 0));
}

void respond(int32_t client_fd) {
    char *res, *buffer = NULL;
    char chunk[ZBUFF_CHUNK_SIZE];
    request_t *req;
    methods m;
    endpoint_t *e;
    ssize_t n;
    size_t total = 0;

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

    if (!req)
        return;

    req->client_fd = client_fd;

    m = string_to_method(req->method);
    if (m >= METHODS_COUNT) {
        free_request(req);
        return;
    }

    e = find_endpoint(m, req->target);
    if (!e) {
        print_debug("%lu : sending 405\n", pthread_self());
        char *(*h)(request_t *) = get_request_error_handler(RES_STATUS_METHOD_NOT_ALLOWED);
        if (!h) {
            free_request(req);
            return;
        }
        char *msg = h(req);
        if (!msg)
            perror("Message creation");
        print_debug("%lu : 405 to be sent:\n%s\n", pthread_self(), msg);
        _send_response(client_fd, msg, strlen(msg), req->method, req->target);
        free(msg);
        free_request(req);
        return;
    }
    req->endpoint = e;

    res = e->handler(req);
    print_debug("%lu : response below\n", pthread_self());
    print_debug("%lu : %s\n", pthread_self(), res);
    print_debug("%lu : about to send response\n", pthread_self());
    _send_response(client_fd, res, strlen(res), req->method, req->target);
    print_debug("%lu : just sent response\n", pthread_self());
    free_request(req);

    if (res)
        free(res);
    print_debug("%lu : finished response process\n", pthread_self());
}

void free_request(request_t *req) {
    if (!req)
        return;
    if (req->method)
        free(req->method);
    if (req->target)
        free(req->target);
    if (req->headers)
        free(req->headers);
    if (req->content_type)
        free(req->content_type);
    if (req->body)
        free(req->body);
    free(req);
}
