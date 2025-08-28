#include "test_utils.h"
#include "fixtures.h"
#include "server/api/socket.h"
#include "server/api/endpoint.h"
#include "server/api/response.h"
#include "server/api/errors.h"
#include "common/status.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>


int8_t test_server_api_create_socket() {
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork failed");
        return (1);
    }
    else if (pid == 0) { // child
        init_jobs_queue();
        init_server_socket_conn(5555, false);
        thread_pool_t *tp = init_thread_pool(2);
        server_loop(tp);
        close_server_socket_conn();
        destroy_thread_pool(tp);
        delete_jobs_queue();
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
    static char *msg = "405 Method not allowed";
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
