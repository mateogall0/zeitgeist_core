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


char *example_handler1(request_t *r)
{
	(void)r;
	char *msg = calloc(20, sizeof(char));
	if (!msg)
		return (NULL);
    strcpy(msg, "H3110 W0r1d");
    return msg;
}


int main()
{
	setvbuf(stdout, NULL, _IONBF, 0);  // disables buffering for other threads
	init_endpoints_list();
	set_endpoint(GET, "/hello/world", example_handler);
	pall_endpoints();

	init_jobs_queue();
	thread_pool_t *tp = init_thread_pool(4);

	init_server_socket_conn((uint32_t)arc4random() % 50000, 4);
	server_loop(tp);

	close_server_socket_conn();
	destroy_endpoints();
	destroy_thread_pool(tp);
	delete_jobs_queue();

	return (0);
}
