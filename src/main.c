#include "thread.h"
#include "queue.h"
#include "socket.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


int main()
{
	server_socket_conn_t *ssc = init_server_socket_conn(8000, 1);
	server_loop(ssc);
	close_server_socket_conn(ssc);
	return (0);
}
