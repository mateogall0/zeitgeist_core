#ifndef ZSERVER_MASTER_H
#define ZSERVER_MASTER_H

#include "server/thread.h"
#include <stdbool.h>
#include <stddef.h>
#include <time.h>


void
initialize_sessions_structure(time_t idle_timout,
                              size_t map_size);

void
run_core_server_loop(uint32_t server_port,
                     uint64_t batch_size,
                     bool verbose,
                     void (*handle_input)(int client_fd));

void
run_server_batches(uint64_t batch_size,
                   bool verbose,
                   void (*handle_input)(int client_fd));

void
clear_core_server_loop(bool verbose);

#endif
