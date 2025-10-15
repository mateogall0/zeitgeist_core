#include "client/connection.h"
#include "common/methods.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


connection_t *
init_conn(void(*process)(char *, size_t)) {
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
    c->connected = 1;

    return (0);
}

ssize_t
send_payload(connection_t *c, char *payload, size_t len) {
    if (!c || !c->connected || !payload || len == 0) {
        errno = EINVAL;
        return (-1);
    }
    return (send(c->client, payload, len, 0));
}

raw_received_payload_t *
client_conn_recv(connection_t *c) {
    if (!c || !c->connected)
        return (NULL);

    struct timeval tv = {0, 100000}; // 0.1 sec
    setsockopt(c->client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    char temp_buf[CLIENT_BUFFER_SIZE];
    size_t total_len = 0;
    char *data = NULL;

    while (1) {
        ssize_t bytes_received = recv(c->client, temp_buf, CLIENT_BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break; // timeout, exit immediately
            free(data);
            return (NULL);
        }
        if (bytes_received == 0)
            break;

        char *new_data = realloc(data, total_len + bytes_received + 1);
        if (!new_data) {
            free(data);
            return (NULL);
        }
        data = new_data;
        memcpy(data + total_len, temp_buf, bytes_received);
        total_len += bytes_received;
        data[total_len] = '\0';

        if (bytes_received < CLIENT_BUFFER_SIZE)
            break;
    }

    if (!data)
        return (NULL);

    raw_received_payload_t *received = malloc(sizeof(raw_received_payload_t));
    if (!received) {
        free(data);
        return (NULL);
    }

    received->data = data;
    received->len = total_len;
    return (received);
}


void
client_conn_loop(connection_t *c) {
    size_t bytes_received;
    char buffer[CLIENT_BUFFER_SIZE];

    if (!c || !c->connected || !c->process)
        return;
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

    if (len == -1) {
        perror("asprintf failed");
        return (0); // alloc failed
    }

    sent_count = send_payload(c, buf, len);
    if (sent_count < 0)
        perror("Send failed");
    free(buf);

    return (sent_count);
}

void destroy_conn(connection_t *conn) {
    if (!conn)
        return;
    disconnect(conn);
    free(conn);
}
