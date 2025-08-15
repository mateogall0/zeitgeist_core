#include "queue.h"
#include "thread.h"
#include "debug.h"
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


jobs_queue_t *jobs_queue = NULL;

jobs_queue_t *init_jobs_queue()
{
	if (jobs_queue)
		return (NULL);
	jobs_queue = (jobs_queue_t *)malloc(sizeof(jobs_queue_t));

	if (!jobs_queue)
		return (NULL);
	jobs_queue->size = 0;
	jobs_queue->head = NULL;
	jobs_queue->tail = NULL;
	return (jobs_queue);
}

void delete_jobs_queue()
{
	job_t *current_job;
	if (!jobs_queue)
		return;

	while (jobs_queue->size > 0)
	{
		current_job = pop_job(jobs_queue);
		free_job(current_job);
	}
	free(jobs_queue);
}

job_t *pop_job()
{
	job_t *tail;

	if (!jobs_queue  || !jobs_queue->tail)
		return (NULL);

	tail = jobs_queue->tail;

	if (tail == jobs_queue->head)
	{
		jobs_queue->head = NULL;
		jobs_queue->tail = NULL;
	}
	else
	{
		jobs_queue->tail = tail->prev;
		tail->prev->next = NULL;
		tail->prev = NULL;
	}


	jobs_queue->size -= 1;
	print_debug("popped a job, queue size: %lu\n", jobs_queue->size);
	return (tail);
}

size_t push_job(job_t *job, thread_pool_t *pool)
{
	if (!jobs_queue)
		return (0);
	if (!job)
		return (jobs_queue->size);
	if (jobs_queue->head)
		jobs_queue->head->prev = job;
	if (!jobs_queue->tail)
		jobs_queue->tail = job;
	job->next = jobs_queue->head;
	job->prev = NULL;
	jobs_queue->size += 1;
	jobs_queue->head = job;

	pthread_cond_signal(&pool->cond);

	print_debug("pushed a job, queue size: %lu\n", jobs_queue->size);
	return (jobs_queue->size);
}

int32_t pall_queue()
{
	job_t *current;
	int32_t count = 0;

	if (!jobs_queue)
		return (-1);

	current = jobs_queue->head;

	while (current) {
		count += print_job(current);
		current = current->next;
	}
	return (count);
}

int32_t print_job(job_t *job)
{
	return printf("Job:\n\tid : %p\n\tfn : %p\n", &job, &job->func);
}

void free_job(job_t *job)
{
	if (!job)
		return;
	free(job);
}

job_t *create_job(void (*func)(int32_t client_fd), int32_t client_fd)
{
	job_t *new_job;

	if (!func)
		return (NULL);

	new_job = (job_t *)calloc(1, sizeof(job_t));
	if (!new_job)
		return (NULL);

	new_job->func = func;
	new_job->next = NULL;
	new_job->prev = NULL;
	new_job->client_fd = client_fd;

	return (new_job);
}
