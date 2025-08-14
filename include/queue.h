#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

typedef struct thread_pool_s thread_pool_t;

#include <stdint.h>
#include <stddef.h>



typedef struct job_s
{
	void (*func)(int32_t client_fd, char *buffer);
	char *data;
	int32_t client_fd;
	struct job_s *next;
	struct job_s *prev;
} job_t;

typedef struct
{
	job_t *head;
	job_t *tail;
	size_t size;
} jobs_queue_t;

jobs_queue_t *init_jobs_queue();
void delete_jobs_queue();
job_t *pop_job();
size_t push_job(job_t *job, thread_pool_t *pool);
int32_t pall_queue();
int32_t print_job(job_t *job);
void free_job(job_t *job);

job_t *create_job(void (*func)(int32_t client_fd, char *buffer), int32_t client_fd, char *data);


extern jobs_queue_t *jobs_queue;

#endif
