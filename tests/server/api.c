#include "test_utils.h"
#include "server/api/socket.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>


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
	}
	return (0);
}
