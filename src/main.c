#include "thread.h"
#include "stdio.h"
#include <stdint.h>


int main()
{
	uint32_t i;
	size_t size = 4;
	thread_pool_t *pool = init_thread_pool(size);
	for (i = 0; i <size; i++)
		printf("%p\n", &pool->threads[i]);
	destroy_thread_pool(pool);
	return (0);
}
