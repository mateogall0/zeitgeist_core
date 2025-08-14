#ifndef THREAD_H
#define THREAD_H

#include "queue.h"

#include <pthread.h>

typedef struct {
    pthread_t *threads;
	size_t size;
	jobs_queue_t *queue;
	pthread_mutex_t lock;
    pthread_cond_t cond;
	int32_t stop;
} thread_pool_t;


thread_pool_t *init_thread_pool(size_t size, jobs_queue_t *queue);
void destroy_thread_pool(thread_pool_t* pool);
void *worker_loop(void *arg);

#endif
