#ifndef ZTEST_FIXTURES_H
#define ZTEST_FIXTURES_H

#include "zeitgeist_client.h"

#define SERVER_PORT 60132

#define SETUP_CLIENT_QUEUE() \
	init_client_payload_queue(128)
#define TEARDOWN_CLIENT_QUEUE_() \
	destroy_client_payload_queue()


#define SETUP_SERVER_SOCKET() \
	pid_t pid; \
	thread_pool_t *tp; \
	if ((pid = fork()) < 0) { \
		perror("fork failed (fixture)"); \
		return (1); \
	} \
	else if (pid == 0) { \
		init_jobs_queue(); \
		init_server_socket_conn(SERVER_PORT, false); \
		tp = init_thread_pool(2); \
	}

#define RUN_SERVER_SOCKET_LOOP() \
	if (pid == 0) \
		server_loop(tp)

#define DESTROY_SERVER_SOCKET() \
	if (pid == 0) { \
		destroy_thread_pool(tp); \
		delete_jobs_queue(); \
		close_server_socket_conn(); \
		exit(0); \
	} \
	kill(pid, SIGINT); \
	waitpid(pid, NULL, 0); \


#endif
