#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H
#include <stdint.h>
#include <stddef.h>

typedef struct job_s{
	int data;
	struct job_s *next;
	struct job_s *prev;
} job_t;

typedef struct {
	job_t *head;
	job_t *tail;
	size_t size;
} jobs_queue_t;

jobs_queue_t *init_jobs_queue();
void delete_jobs_queue(jobs_queue_t *jobs_queue);
job_t *pop_job(jobs_queue_t *jobs_queue);
size_t push_job(jobs_queue_t *jobs_queue, job_t *job);
int32_t pall_queue(jobs_queue_t *queue);
int32_t print_job(job_t *job);
void free_job(job_t *job);

#endif
