#include "test_utils.h"
#include "zeitgeist_client.h"



int test_queue_creation()
{
	init_client_payload_queue(5);
	ASSERT(client_payload_queue->size == 0);
	push_client_payload("Hello\n", 6);
	ASSERT(client_payload_queue->size ==  1);
	destroy_client_payload_queue();

	return (0);
}
