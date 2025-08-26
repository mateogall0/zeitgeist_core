#include "test_utils.h"
#include "fixtures.h"
#include "server/api/socket.h"
#include "server/api/endpoint.h"
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
		sleep(1);
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
