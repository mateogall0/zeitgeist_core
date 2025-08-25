#ifndef THREAD_H
#define THREAD_H

#include "server/queue.h"
#include <pthread.h>
#include <stdint.h>

typedef struct thread_pool_s{
	pthread_t *threads;
	size_t size;
	jobs_queue_t *queue;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	pthread_cond_t cond_empty;
	int32_t stop;
	uint32_t active_workers;
} thread_pool_t;


thread_pool_t *init_thread_pool(size_t size);
void destroy_thread_pool(thread_pool_t* pool);
void *worker_loop(void *arg);

#endif
