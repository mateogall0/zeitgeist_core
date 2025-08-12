#include "thread.h"
#include "queue.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


int main()
{
	jobs_queue_t *q = init_jobs_queue();

	for (int i = 0; i < 10; i++)
	{
		job_t *j = malloc(sizeof(job_t));
		push_job(q, j);
	}

	pall_queue(q);

	delete_jobs_queue(q);
	return (0);
}
