#include "server/queue.h"
#include "server/thread.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>


thread_pool_t *init_thread_pool(size_t size) {
    thread_pool_t *new_pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
    uint32_t i;

    if (!new_pool)
        return (NULL);

    new_pool->threads = (pthread_t *)calloc(size, sizeof(pthread_t));

    if (!new_pool->threads) {
        free(new_pool);
        return (NULL);
    }

    pthread_mutex_init(&new_pool->lock, NULL);
    pthread_cond_init(&new_pool->cond, NULL);
    pthread_cond_init(&new_pool->cond_empty, NULL);


    new_pool->size = size;
    new_pool->stop = 0;
    new_pool->active_workers = 0;

    new_pool->queue = jobs_queue;
    for (i = 0; i < size; i++)
        pthread_create(&new_pool->threads[i], NULL, worker_loop, new_pool);

    return (new_pool);
}

void destroy_thread_pool(thread_pool_t *pool) {
    if (!pool)
        return;

    // signal all workers to stop
    pthread_mutex_lock(&pool->lock);
    pool->stop = 1;
    pthread_cond_broadcast(&pool->cond); // wake all threads
    pthread_mutex_unlock(&pool->lock);

    while (pool->active_workers > 0 || pool->queue->size > 0)
        pthread_cond_wait(&pool->cond_empty, &pool->lock);

    pthread_mutex_unlock(&pool->lock);

    // wait for all threads to exit
    for (size_t i = 0; i < pool->size; i++)
        pthread_join(pool->threads[i], NULL);

    // now it is safe to destroy mutexes and free memory
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);
    pthread_cond_destroy(&pool->cond_empty);

    free(pool->threads);

    // free the queue only **after** workers exited
    delete_jobs_queue(pool->queue);

    free(pool);
}

void *worker_loop(void *arg) {
    thread_pool_t *pool = arg;
    if (!pool) return NULL;

    while (1) {
        pthread_mutex_lock(&pool->lock);

        // wait while queue empty AND not stopping
        while (pool->queue->size == 0 && !pool->stop)
            pthread_cond_wait(&pool->cond, &pool->lock);

        // exit immediately if stop flag set
        if (pool->stop && pool->queue->size == 0) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        // pop a job if available
        job_t *job = pop_job(pool->queue);
        if (job)
            pool->active_workers++;

        pthread_mutex_unlock(&pool->lock);

        if (job) {
            job->func(job->client_fd);
            free_job(job);

            pthread_mutex_lock(&pool->lock);
            pool->active_workers--;
            // notify destroy or any waiters that queue is empty
            if (pool->queue->size == 0 && pool->active_workers == 0)
                pthread_cond_signal(&pool->cond_empty);
            pthread_mutex_unlock(&pool->lock);
        }
    }

    return NULL;
}
