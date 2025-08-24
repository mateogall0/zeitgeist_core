#include "test_utils.h"
#include "server/queue.h"


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
