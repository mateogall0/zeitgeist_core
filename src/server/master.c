#include "server/master.h"
#include "server/api/socket.h"
#include "server/api/endpoint.h"
#include "server/thread.h"
#include "server/sessions/map.h"
#include "server/sessions/wheel.h"
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>


/**
 * If `idle_timeout` is zero or lower, the connected sessions
 * wheel and map do not get initialized
 */
void
initialize_sessions_structure(time_t idle_timout,
                              size_t map_size) {
    if (idle_timout <= 0) {
        fprintf(stderr, "Idle timeout must be greater than zero\n");
        return;
    }
    if (map_size == 0) {
        fprintf(stderr, "Map size must be greater than zero\n");
        return;
    }


    if (!init_connected_sessions_wheel(idle_timout)) {
        fprintf(stderr, "Error initializing sessions wheel\n");
        return;
    }

    if (!init_connected_sessions_map(map_size)) {
        fprintf(stderr, "Error initializing sessions map\n"
                        "Clearing wheel\n");
        destroy_connected_sessions_wheel();
        return;
    }
}

/**
 * The endpoints are initialized and stated prior to this
 */
void
run_core_server_loop(uint32_t server_port,
                     uint64_t batch_size,
                     bool verbose,
                     void (*handle_input)(int client_fd)) {
    if (verbose) {
        printf("Initialized endpoints:\n");
        if (pall_endpoints() <= 0)
            printf("No endpoints initialized");
    }
    init_jobs_queue();
    init_server_socket_conn(server_port, verbose);
    run_server_batches(batch_size, verbose, handle_input);
}

void
run_server_batches(uint64_t batch_size,
                   bool verbose,
                   void (*handle_input)(int client_fd)) {
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
            server_loop(handle_input);
            if (verbose)
                printf("Clearing server at %d...\n", pid);
            clear_core_server_loop(verbose);
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
    clear_core_server_loop(verbose);
    if (verbose)
        puts("Parent cleared");
}

void
clear_core_server_loop(bool verbose) {
    delete_jobs_queue();
    close_server_socket_conn();
    destroy_endpoints();
    if (verbose)
        puts("All cleared");
}
