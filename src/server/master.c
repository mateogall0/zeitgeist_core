#include "server/master.h"
#include "server/api/socket.h"
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

void
run_core_server_loop(uint64_t batch_size, size_t thread_pool_size, bool verbose) {
    thread_pool_t *pool;
    init_jobs_queue();
    pool = init_thread_pool(2);
    run_server_batches(batch_size, pool, verbose);
}

void
run_server_batches(uint64_t batch_size,
                   thread_pool_t *pool,
                   bool verbose) {
    if (!scc) {
        if (verbose) fprintf(stderr, "Server socket not initialized\n");
        return;
    }
    pid_t pids[batch_size];

    for (uint64_t i = 0; i < batch_size; ++i) {
        pid_t pid = fork();
        pids[i] = pid;
        if (pid < 0) {
            if (verbose)
                fprintf(stderr, "Fork failed when running batch %d\n", pid);
        }
        else if (pid == 0) {
            if (verbose)
                printf("Starting server loop nº%d...\n", pid);
            server_loop(pool);
            clear_core_server_loop(pool, verbose);
            exit(0);
        }
    }
    for (uint64_t i = 0; i < batch_size; ++i) {
        int status;
        pid_t child = wait(&status);
        if (verbose)
            printf("Child %d terminated\n", child);
    }
    if (verbose)
        puts("All children are done");
}

void
clear_core_server_loop(thread_pool_t *pool,
                       bool verbose) {
    if (ssc) {
    }
}
