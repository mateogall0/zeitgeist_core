#include "test_utils.h"
#include "server/queue.h"
#include "server/thread.h"


int8_t test_server_queue_creation() {
    ASSERT(!jobs_queue);
    jobs_queue_t *jq = init_jobs_queue();
    ASSERT(jq);
    ASSERT(!jq->head);
    ASSERT(!jq->tail);
    ASSERT(jq->size == 0);

    ASSERT(jobs_queue == jq);
    delete_jobs_queue();
    ASSERT(!jobs_queue);

    return (0);
}

#define _TEST_JOBS_SIZE 50

int32_t _jobs_result[_TEST_JOBS_SIZE];

void _process(int32_t client_fd) {
    _jobs_result[client_fd] = client_fd * 2;
}

int8_t _test_server_queue_push_pop__threaded(size_t thread_pool_size) {
    init_jobs_queue();
    thread_pool_t *tp = init_thread_pool(thread_pool_size);

    for (size_t i = 0; i < _TEST_JOBS_SIZE; ++i) {
        job_t *j = create_job(_process, i);
        push_job(j, tp);
    }

    pthread_mutex_lock(&tp->lock);
    while (tp->queue->size > 0 || tp->active_workers > 0)
        pthread_cond_wait(&tp->cond_empty, &tp->lock);
    pthread_mutex_unlock(&tp->lock);

    for (int32_t i = 0; i < _TEST_JOBS_SIZE; ++i)
        ASSERT(_jobs_result[i] == i * 2);

    destroy_thread_pool(tp);
    return (0);
}
