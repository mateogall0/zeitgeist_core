#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

jobs_queue_t *init_jobs_queue() {
	jobs_queue_t *queue = (jobs_queue_t *)malloc(sizeof(jobs_queue_t));

	if (!queue)
		return (NULL);
	queue->size = 0;
	queue->head = NULL;
	queue->tail = NULL;
	return (queue);
}

void delete_jobs_queue(jobs_queue_t *jobs_queue) {
	job_t *current_job;
	if (!jobs_queue)
		return;

	while (jobs_queue->size > 0) {
		current_job = pop_job(jobs_queue);
		free_job(current_job);
	}
	free(jobs_queue);
}

job_t *pop_job(jobs_queue_t *jobs_queue) {
	job_t *tail;

	if (!jobs_queue  || !jobs_queue->tail)
		return (NULL);

	tail = jobs_queue->tail;

	if (tail == jobs_queue->head) {
		jobs_queue->head = NULL;
		jobs_queue->tail = NULL;
	} else {
		jobs_queue->tail = tail->prev;
		tail->prev->next = NULL;
		tail->prev = NULL;
	}

	
	jobs_queue->size -= 1;
	return (tail);
}

size_t push_job(jobs_queue_t *jobs_queue, job_t *job) {
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
	return (jobs_queue->size);
}

int32_t pall_queue(jobs_queue_t *queue) {
	job_t *current;
	int32_t count = 0;

	if (!queue)
		return (-1);

	current = queue->head;

	while (current) {
		count += print_job(current);
		current = current->next;
	}
	return (count);
}

int32_t print_job(job_t *job) {
	return printf("Job:\n\tid : %p\n", &job);
}
void free_job(job_t *job) {
	if (!job)
		return;
	free(job);
}
