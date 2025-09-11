#ifndef ZTEST_FIXTURES_H
#define ZTEST_FIXTURES_H

#include "zeitgeist_client.h"

#define SERVER_PORT 4000

#define SETUP_CLIENT_QUEUE()                    \
    init_client_payload_queue(128)
#define TEARDOWN_CLIENT_QUEUE_()                \
    destroy_client_payload_queue()


#define SETUP_SERVER_SOCKET()                               \
    int ztest_pipefd[2];                                    \
    pid_t pid;                                              \
    if (pipe(ztest_pipefd) < 0) {                           \
        perror("pipe failed");                              \
        return 1;                                           \
    }                                                       \
    if ((pid = fork()) < 0) {                               \
        perror("fork failed (fixture)");                    \
        return 1;                                           \
    } else if (pid == 0) {                                  \
        close(ztest_pipefd[0]); /* child closes read end */ \
        init_server_socket_conn(SERVER_PORT, false);        \
        /* signal parent that server is ready */            \
        write(ztest_pipefd[1], "1", 1);                     \
        close(ztest_pipefd[1]);                             \
    }

#define RUN_SERVER_SOCKET_LOOP()                    \
    if (pid == 0) {                                 \
        server_loop(respond);                       \
        close_server_socket_conn();                 \
        exit(0);                                    \
    } else {                                        \
        /* parent waits for child to be ready */    \
        close(ztest_pipefd[1]);                     \
        char ztest_buf;                             \
        read(ztest_pipefd[0], &ztest_buf, 1);       \
        close(ztest_pipefd[0]);                     \
    }

#define DESTROY_SERVER_SOCKET()                 \
    kill(pid, SIGINT);                          \
    waitpid(pid, NULL, 0);


#endif
