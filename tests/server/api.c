#include "tests.h"
#include "fixtures.h"
#include "server/api/socket.h"
#include "server/api/endpoint.h"
#include "server/api/response.h"
#include "server/api/errors.h"
#include "server/sessions/wheel.h"
#include "server/sessions/map.h"
#include "common/status.h"
#include "utils.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <stdint.h>


int8_t test_server_api_create_socket() {
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork failed");
        return (1);
    }
    else if (pid == 0) { // child
        init_server_socket_conn(5555, false);
        server_loop(respond);
        close_server_socket_conn();
        exit(0);
    }
    else { // parent
        kill(pid, SIGINT);
        waitpid(pid, NULL, 0);
    }
    return (0);
}

char *_process_endpoint_example(request_t *req) {
    (void)req;
    return (strdup("response_example"));
}

int8_t test_server_api_create_endpoints() {
    ASSERT(!endpoints);
    init_endpoints_list();
    ASSERT(endpoints);
    ASSERT(endpoints->head == NULL);

    for (methods m = GET; m != METHODS_COUNT; m++) {
        char t[] = "/example0";
        t[8] += (int8_t)m;
        endpoint_t *e = set_endpoint(m, t, _process_endpoint_example);
        ASSERT(e);
        if (m == GET)
            ASSERT(endpoints->head == e);
        ASSERT(strcmp(e->target, t) == 0);
        ASSERT(e->method == m);
    }
    SETUP_SERVER_SOCKET();

    RUN_SERVER_SOCKET_LOOP();

    DESTROY_SERVER_SOCKET();
    destroy_endpoints();
    return (0);
}


char *_process_error_example_405(request_t *r) {
    (void)r;
    char *msg = strdup("405 Method not allowed");
    return (msg);
}

int8_t test_server_api_request_single_endpoint() {
    ASSERT(init_endpoints_list());
    ASSERT(init_request_errors_list());
    ASSERT(set_request_error(RES_STATUS_METHOD_NOT_ALLOWED,
                             _process_error_example_405));
    endpoint_t *e = set_endpoint(GET, "/example", _process_endpoint_example);
    ASSERT(e);

    SETUP_SERVER_SOCKET();

    RUN_SERVER_SOCKET_LOOP();

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
         perror("connect");
         return 1;
    }

    char *msg = "GET /example";
    send(sock, msg, strlen(msg), 0);

    char buf[1024];
    int n = recv(sock, buf, sizeof(buf)-1, 0);
    buf[n] = '\0';
    ASSERT(strcmp(buf, "response_example") == 0);

    msg = "POST /example";
    if (send(sock, msg, strlen(msg), 0) < 0)
        return (1);

    n = recv(sock, buf, sizeof(buf)-1, 0);
    buf[n] = '\0';
    ASSERT(strcmp(buf, "405 Method not allowed") == 0);


    close(sock);

    DESTROY_SERVER_SOCKET();
    destroy_endpoints();
    destroy_request_errors_list();
    return (0);
}

int8_t test_server_api_request_many_connections(uint32_t connections) {
    SETUP_SERVER_SOCKET();
    RUN_SERVER_SOCKET_LOOP();

    int sockets[connections];

    for (uint32_t i = 0; i < connections; ++i) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        sockets[i] = sock;

        struct sockaddr_in addr = {0};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(SERVER_PORT);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1

        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("connect");
            return 1;
        }

    }
    for (uint32_t i = 0; i < connections; ++i) {
        close(sockets[i]);
    }

    DESTROY_SERVER_SOCKET();
    destroy_endpoints();
    destroy_request_errors_list();
    return (0);
}

char *_echo_request(request_t *r) {
    char *msg = strdup(r->body);
    return (msg);
}


int8_t _test_server_api_echo_random_payload(size_t payload_length) {
    init_endpoints_list();
    set_endpoint(GET, "/example", _echo_request);
    ASSERT(endpoints);
    SETUP_SERVER_SOCKET();
    RUN_SERVER_SOCKET_LOOP();

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
         perror("connect");
         return 1;
    }

    size_t msg_len = strlen("GET /example\r\n\r\n");
    size_t body_len = payload_length;
    char *msg = malloc(msg_len + body_len + 1);
    if (!msg) {
        perror("malloc");
        exit(1);
    }

    memcpy(msg, "GET /example\r\n\r\n", msg_len + 1);
    char *body = random_string(payload_length);
    strcat(msg, body);

    if (send(sock, msg, strlen(msg), 0) < 0)
        return (1);

    char buf[payload_length + 1];
    int n = recv(sock, buf, sizeof(buf)-1, 0);
    buf[n] = '\0';
    ASSERT(strcmp(buf, body) == 0);

    free(body);
    free(msg);

    close(sock);

    DESTROY_SERVER_SOCKET();
    destroy_endpoints();
    return (0);
}

char *_manual_broadcast_request(request_t *r) {
    char *broadcast_message = strdup("Hello from the broadcast");
    for (size_t i = 0; i < connected_sessions_map->size; ++i) {
        if (!connected_sessions_map->_map[i])
            continue;
        int client_fd = connected_sessions_map->_map[i]->client_fd;
        if (!client_fd || r->client_fd == client_fd)
            continue;
        send_unrequested_payload(client_fd,
                                 broadcast_message,
                                 strlen(broadcast_message));
    }
    free(broadcast_message);
    char *msg = strdup("done");
    return (msg);
}

int8_t test_send_unrequested_payload() {
    init_connected_sessions_map(1024);
    init_connected_sessions_wheel(1024);
    init_endpoints_list();
    set_endpoint(POST, "/broadcast", _manual_broadcast_request);
    SETUP_SERVER_SOCKET();
    RUN_SERVER_SOCKET_LOOP();

    // --- first client (will receive broadcast) ---
    int sock0 = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr0 = {0};
    addr0.sin_family = AF_INET;
    addr0.sin_port = htons(SERVER_PORT);
    addr0.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock0, (struct sockaddr*)&addr0, sizeof(addr0)) < 0) {
         perror("connect sock0");
         return 1;
    }

    // --- second client (will send broadcast request) ---
    int sock1 = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr1 = {0};
    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(SERVER_PORT);
    addr1.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock1, (struct sockaddr*)&addr1, sizeof(addr1)) < 0) {
         perror("connect sock1");
         return 1;
    }

    // sock1 sends POST /broadcast → triggers broadcast to sock0
    const char *req = "POST /broadcast\r\n\r\n";
    if (send(sock1, req, strlen(req), 0) < 0) {
        perror("send sock1");
        return 1;
    }

    // sock1 should get "done"
    char buf1[128];
    int n1 = recv(sock1, buf1, sizeof(buf1)-1, 0);
    if (n1 <= 0) {
        perror("recv sock1");
        return 1;
    }
    buf1[n1] = '\0';
    ASSERT(strcmp(buf1, "done") == 0);

    // sock0 should get the broadcast message
    char buf0[128];
    int n0 = recv(sock0, buf0, sizeof(buf0)-1, 0);
    if (n0 <= 0) {
        perror("recv sock0");
        return 1;
    }
    buf0[n0] = '\0';
    ASSERT(strcmp(buf0, "Hello from the broadcast") == 0);

    close(sock0);
    close(sock1);

    DESTROY_SERVER_SOCKET();
    destroy_endpoints();
    destroy_connected_sessions_map();
    destroy_connected_sessions_wheel();

    return (0);
}
