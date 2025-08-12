#include "thread.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>


thread_pool_t *init_thread_pool(size_t size)
{
	thread_pool_t *new_pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));

	if (!new_pool)
		return (NULL);

	new_pool->threads = (pthread_t *)malloc(size *sizeof(pthread_t));

	if (!new_pool->threads)
	{
		free(new_pool);
		return (NULL);
	}

	new_pool->size = size;
	return (new_pool);
}

void destroy_thread_pool(thread_pool_t *pool)
{
	free(pool->threads);
	free(pool);
}
