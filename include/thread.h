#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

typedef struct {
    pthread_t *threads;
	size_t size;
} thread_pool_t;


thread_pool_t *init_thread_pool(size_t size);
void destroy_thread_pool(thread_pool_t* pool);


#endif
