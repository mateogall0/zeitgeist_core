#include "queue.h"
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
	uint32_t i;

	if (!new_pool)
		return (NULL);

	new_pool->threads = (pthread_t *)calloc(size, sizeof(pthread_t));

	if (!new_pool->threads)
	{
		free(new_pool);
		return (NULL);
	}

	pthread_mutex_init(&new_pool->lock, NULL);
    pthread_cond_init(&new_pool->cond, NULL);

	new_pool->size = size;
	new_pool->stop = 0;

	new_pool->queue = jobs_queue;
	for (i = 0; i < size; i++)
		pthread_create(&new_pool->threads[i], NULL, worker_loop, new_pool);

	return (new_pool);
}

void destroy_thread_pool(thread_pool_t *pool)
{
    if (!pool)
        return;

    pthread_mutex_lock(&pool->lock);
    pool->stop = 1;
    pthread_cond_broadcast(&pool->cond); // wake all threads
    pthread_mutex_unlock(&pool->lock);

    for (size_t i = 0; i < pool->size; i++)
	{
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);

    free(pool->threads);
    free(pool);
}

void *worker_loop(void *arg)
{
    thread_pool_t *pool = arg;

	if (!pool)
		return (NULL);

	for(;;)
	{
        pthread_mutex_lock(&pool->lock);
        while (pool->queue->size == 0 && !pool->stop)
            pthread_cond_wait(&pool->cond, &pool->lock);

        if (pool->stop)
		{
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        job_t *job = pop_job(pool->queue);
        pthread_mutex_unlock(&pool->lock);

        if (job)
		{
            /* printf("Thread %lu processing job %d\n", pthread_self(), job->data); */
            job->func(job->client_fd, job->data);
            free_job(job);
        }
    }
	return (NULL);
}
