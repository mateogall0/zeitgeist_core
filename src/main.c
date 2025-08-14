#include "thread.h"
#include "queue.h"
#include "api/socket.h"
#include "api/endpoint.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


char *example_handler(request_t *r)
{
	(void)r;
	char *msg = calloc(20, sizeof(char));
	if (!msg)
		return (NULL);
    strcpy(msg, "Hello World");
    return msg;
}


int main()
{
	init_endpoints_list();
	set_endpoint(GET, "/hello/world", example_handler);
	pall_endpoints();

	init_jobs_queue();
	thread_pool_t *tp = init_thread_pool(4);

	init_server_socket_conn(8000, 1);
	server_loop(tp);

	close_server_socket_conn();
	destroy_endpoints();
	destroy_thread_pool(tp);
	delete_jobs_queue();

	return (0);
}
