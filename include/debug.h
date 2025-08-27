#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define print_debug(fmt, ...) fprintf(stderr, "Debug: " fmt, ##__VA_ARGS__)
#else
#define print_debug(fmt, ...) // DEBUG is not defined
#endif


#endif
