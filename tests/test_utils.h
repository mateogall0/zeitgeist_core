#ifndef ZTEST_UTILS_H
#define ZTEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "\nAssertion failed in %s: %s\n", __func__, #cond); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

#define RUN_TEST(fn) do { \
    if (fn() == 0) { \
        printf("."); \
        fflush(stdout); \
    } else { \
        printf("E\n"); \
        return 1; \
    } \
} while (0)


/**
 * All test functions go below here.
 * The functions are supposed to work as intended, otherwise
 * the `ASSERT_EQ` macro will exit on failure with status code
 * `EXIT`
 */
int test_queue_creation();


#endif
