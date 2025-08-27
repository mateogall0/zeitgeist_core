#ifndef DEBUG_H
#define DEBUG_H


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#ifdef DEBUG
#define print_debug(fmt, ...) do {                                      \
        FILE *tty = fopen("/dev/tty", "w");                             \
        if (tty) {                                                      \
            fprintf(tty, "[pid %d] " fmt, getpid(), ##__VA_ARGS__);     \
            fclose(tty);                                                \
        }                                                               \
    } while (0)
#else
#define print_debug(fmt, ...) ((void)0)
#endif


#endif
