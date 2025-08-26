#ifndef ZTEST_UTILS_H
#define ZTEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define _COLOR_RED     "\x1b[31m"
#define _COLOR_GREEN   "\x1b[32m"
#define _COLOR_RESET   "\x1b[0m"

// Assert that a condition is true. On failure, print the condition and exit.
#define ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, _COLOR_RED "\nAssertion failed in %s (%s:%d): %s\n" _COLOR_RESET, \
                __func__, __FILE__, __LINE__, #cond); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

// Run a test function, print green dot on success, red E and exit on failure
#define RUN_TEST(res) do { \
    if (res == 0) { \
        printf(_COLOR_GREEN "." _COLOR_RESET); \
        fflush(stdout); \
    } else { \
        printf(_COLOR_RED "E\n" _COLOR_RESET); \
        return (1);			       \
    } \
} while (0)

/**
 * All test functions go below here.
 * The functions are supposed to work as intended, otherwise
 * the `ASSERT_EQ` macro will exit on failure with status code
 * `EXIT`
 */

/* Client queue */
int8_t test_client_queue_creation();
int8_t test_client_queue_capacity();
int8_t test_client_queue_pop();
int8_t test_client_queue_pop_all();

/* Server queue */
int8_t test_server_queue_creation();
int8_t _test_server_queue_push_pop__threaded(size_t thread_pool_size);

/* Server API */
int8_t test_server_api_create_socket();

/* All tests to be run go here */
#define TESTS \
	RUN_TEST(test_client_queue_creation()); \
	RUN_TEST(test_client_queue_capacity()); \
	RUN_TEST(test_client_queue_pop()); \
	RUN_TEST(test_client_queue_pop_all()); \
	RUN_TEST(test_server_queue_creation()); \
	RUN_TEST(test_server_api_create_socket()); \
	for (int32_t i = 1; i <= 4; i *= 2) \
		RUN_TEST(_test_server_queue_push_pop__threaded(i)); \


#endif
