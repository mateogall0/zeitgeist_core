#ifndef ZSERVER_MASTER_H
#define ZSERVER_MASTER_H

#include "server/thread.h"
#include <stdbool.h>


void run_core_server_loop(uint32_t server_port,
                          uint64_t batch_size,
                          size_t thread_pool_size,
                          bool verbose);
void run_server_batches(uint64_t batch_size,
                        thread_pool_t *pool,
                        bool verbose);
void clear_core_server_loop(thread_pool_t *pool,
                            bool verbose);


#endif
