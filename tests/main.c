#include "test_utils.h"


int main()
{
	RUN_TEST(test_client_queue_creation);
	RUN_TEST(test_client_queue_capacity);
	RUN_TEST(test_client_queue_pop);
	RUN_TEST(test_client_queue_pop_all);
	RUN_TEST(test_server_queue_creation);

	printf("\nAll tests passed!\n");
	return (0);
}
