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
	static char msg[20];
    strcpy(msg, "Hello World");
    return msg;
}


int main()
{
	init_endpoints_list();
	set_endpoint(GET, "/hello/world", example_handler);
	pall_endpoints();

	init_jobs_queue();


	server_socket_conn_t *ssc = init_server_socket_conn(8000, 1);
	server_loop(ssc);

	close_server_socket_conn(ssc);
	destroy_endpoints();

	return (0);
}
