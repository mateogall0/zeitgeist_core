#include "test_utils.h"
#include "zeitgeist_client.h"
#include "fixtures.h"
#include <string.h>


int8_t test_client_queue_creation()
{
	ASSERT(!client_payload_queue);
	init_client_payload_queue(5);
	ASSERT(client_payload_queue);
	ASSERT(client_payload_queue->size == 0);
	ASSERT(client_payload_queue->capacity == 5);
	ASSERT(!client_payload_queue->head);
	push_client_payload("Hello\n", 6);
	ASSERT(client_payload_queue->size ==  1);
	destroy_client_payload_queue();
	ASSERT(!client_payload_queue);

	return (0);
}

int8_t test_client_queue_capacity()
{
	size_t capacity = 5;
	init_client_payload_queue(capacity);
	for (size_t i = 0; i < capacity; i++)
	{
		push_client_payload("Hello\n", 6);
		ASSERT(client_payload_queue->size == i + 1);
	}
	for (size_t i = 0; i < capacity * 2; i++)
	{
		push_client_payload("Hello\n", 6);
		ASSERT(client_payload_queue->size == capacity);
	}

	destroy_client_payload_queue();

	return (0);
}

int8_t test_client_queue_pop()
{
	size_t capacity = 5;
	init_client_payload_queue(capacity);

	ASSERT(pop_client_payload() == NULL);

	push_client_payload("Hello\n", 6);
	ASSERT(client_payload_queue->size == 1);

	received_payload_t *popped = pop_client_payload();

	ASSERT(popped);
	ASSERT(!popped->next);
	ASSERT(strcmp(popped->data, "Hello\n") == 0);
	ASSERT(popped->len == 6);
	ASSERT(client_payload_queue->size == 0);

	free_received_payload(popped);

	destroy_client_payload_queue();

	return (0);
}


int8_t test_client_queue_pop_all()
{
	size_t capacity = 5, i;
	char *data[] = {
		"Hello",
		"Hello again",
		"This is getting weird",
		"Why?",
		"Sorry, this is unprofessional"
	};
	init_client_payload_queue(capacity);

	ASSERT(pop_client_payload() == NULL);

	for (i = 0; i < capacity; i++)
	{
		push_client_payload(data[i], strlen(data[i]));
		ASSERT(client_payload_queue->size == i + 1);
	}
	for (; i-- > 0; )
	{
		received_payload_t *rp = pop_client_payload();
		ASSERT(rp);
		ASSERT(strcmp(rp->data, data[i]) == 0);
		free_received_payload(rp);
	}

	destroy_client_payload_queue();

	return (0);
}
