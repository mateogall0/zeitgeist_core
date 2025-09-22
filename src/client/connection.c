#include "client/connection.h"
#include "common/methods.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>


connection_t *
init_conn(void(*process)(char *, size_t)) {
    if (!process)
        return (NULL);

    connection_t *c = malloc(sizeof(connection_t));
    if (!c)
        return (NULL);

    c->connected = 0;
    c->client = 0;
    c->process = process;
    return (c);
}

int8_t
connect_client(connection_t *c, char *url, int32_t port) {
    if (!c || c->connected || !url)
        return (1);

    c->client = socket(AF_INET, SOCK_STREAM, 0);
    if (c->client == -1) {
        perror("Socket creation failed");
        return (1);
    }

    c->server_addr.sin_family = AF_INET;
    c->server_addr.sin_port = htons(port);
    inet_pton(AF_INET, url, &c->server_addr.sin_addr);

    if (connect(c->client, (struct sockaddr *)&c->server_addr,
                sizeof(c->server_addr)) < 0) {
        perror("Connection failed");
        return (1);
    }

    return (0);
}

ssize_t
send_payload(connection_t *c, char *payload, size_t len) {
    if (!c || !c->connected || !payload || len == 0)
        return (-1);
    return (send(c->client, payload, len, 0));
}

void
client_conn_loop(connection_t *c) {
    size_t bytes_received;
    char buffer[CLIENT_BUFFER_SIZE];

    if (!c || !c->connected || !c->process) {
        perror("Cannot start the client loop (not connected)");
        return;
    }
    for(;;) {
        bytes_received = recv(c->client, buffer, CLIENT_BUFFER_SIZE, 0);

        if (bytes_received > 0)
            c->process(buffer, bytes_received);
    }
}

int8_t
disconnect(connection_t *c) {
    if (!c || !c->connected)
        return (1);

    c->connected = 0;

    return (close(c->client));
}

ssize_t
send_request_payload(connection_t *c,
                     methods method,
                     char *target,
                     char *headers,
                     unsigned long req_id,
                     char *body) {
    if (!c || !target ||!headers || !body ||
        method < GET || method >= METHODS_COUNT)
        return (0);

    char *buf;
    int len;
    ssize_t sent_count;

    len = asprintf(&buf,
                   "%s %s %lu\r\n"  // method - target - request id
                   "%s\r\n"  // headers
                   "\r\n"  // empty line
                   "%s",  // body content
                   methods_str[method], target, req_id,
                   headers,
                   body);

    if (len == -1)
        return (0); // alloc failed

    sent_count = send_payload(c, buf, len);
    if (sent_count < 0)
        perror("Send failed");
    free(buf);

    return (sent_count);
}
