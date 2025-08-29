#include "server/master.h"
#include "server/api/socket.h"
#include "server/api/endpoint.h"
#include "server/thread.h"
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>


/**
 * The endpoints are initialized and stated prior to it
 */
void
run_core_server_loop(uint32_t server_port,
                     uint64_t batch_size,
                     size_t thread_pool_size,
                     bool verbose) {
    if (verbose) {
        printf("Initialized endpoints:\n");
        if (pall_endpoints() <= 0)
            printf("No endpoints initialized");
    }
    init_jobs_queue();
    init_server_socket_conn(server_port, verbose);
    thread_pool_t *pool = init_thread_pool(thread_pool_size);
    run_server_batches(batch_size, pool, verbose);
}

void
run_server_batches(uint64_t batch_size,
                   thread_pool_t *pool,
                   bool verbose) {
    if (!ssc) {
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
                printf("Starting server loop at %d...\n", pid);
            server_loop(pool);
            if (verbose)
                printf("Clearing server at %d...\n", pid);
            clear_core_server_loop(pool, verbose);
            exit(0);
        }
    }
    for (uint64_t i = 0; i < batch_size; ++i) {
        int status;
        pid_t child = waitpid(pids[i], &status, 0);
        if (verbose)
            printf("Child %d terminated\n", child);
    }
    if (verbose)
        puts("All children are done");
    clear_core_server_loop(pool, verbose);
    if (verbose)
        puts("Parent cleared");
}

void
clear_core_server_loop(thread_pool_t *pool,
                       bool verbose) {
    destroy_thread_pool(pool);
    delete_jobs_queue();
    close_server_socket_conn();
    destroy_endpoints();
    if (verbose)
        puts("All cleared");
}
