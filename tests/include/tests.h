#ifndef ZTEST__H
#define ZTEST__H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define _COLOR_RED     "\x1b[31m"
#define _COLOR_GREEN   "\x1b[32m"
#define _COLOR_RESET   "\x1b[0m"



// Assert that a condition is true. On failure, print the condition and exit.
#define ASSERT(cond) do {                                               \
        if (!(cond)) {                                                  \
            fprintf(stderr, _COLOR_RED "\nAssertion failed in %s (%s:%d): %s\n" _COLOR_RESET, \
                    __func__, __FILE__, __LINE__, #cond);               \
            exit(EXIT_FAILURE);                                         \
        }                                                               \
    } while (0)

// Run a test function, print green dot on success, red E and exit on failure
#define RUN_TEST(fn) do {                                   \
        int _res = fn;                                      \
        printf("%04d || %-70s ", ++_test_counter, #fn);     \
        if (_res == 0) {                                    \
            printf(_COLOR_GREEN "[PASS]\n" _COLOR_RESET);   \
        } else {                                            \
            printf(_COLOR_RED "[FAIL]\n" _COLOR_RESET);     \
            return (1);                                     \
        }                                                   \
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

/* Client unresolved */
int8_t test_client_unresolved_creation();
int8_t test_client_unresolved_pop();

/* Server queue */
/* int8_t test_server_queue_creation(); */
/* int8_t _test_server_queue_push_pop__threaded(size_t thread_pool_size); */

/* Server API */
int8_t test_server_api_create_socket();
int8_t test_server_api_create_endpoints();
int8_t test_server_api_request_single_endpoint();
int8_t test_server_api_request_many_connections(uint32_t connections);
int8_t _test_server_api_echo_random_payload(size_t payload_length);
int8_t test_send_unrequested_payload();

/* Server sessions */
int8_t test_server_sessions_wheel_push();
int8_t test_server_sessions_map_init_zerosize();
int8_t _test_server_sessions_map_push(size_t map_size);

/* Integration request */
int8_t test_request_static_payload();

/* All tests to be run go here */
#define TESTS                                                   \
    RUN_TEST(test_client_unresolved_creation());                \
    RUN_TEST(test_client_unresolved_pop());                     \
    RUN_TEST(test_client_queue_creation());                     \
    RUN_TEST(test_client_queue_capacity());                     \
    RUN_TEST(test_server_sessions_wheel_push());                \
    RUN_TEST(test_client_queue_pop());                          \
    RUN_TEST(test_request_static_payload());                    \
    RUN_TEST(test_server_sessions_map_init_zerosize());         \
    RUN_TEST(test_send_unrequested_payload());                  \
    for (size_t i = 256; i <= 8192; i *= 2)                     \
        RUN_TEST(_test_server_sessions_map_push(i));            \
    RUN_TEST(test_client_queue_pop_all());                      \
    RUN_TEST(test_server_api_create_socket());                  \
    RUN_TEST(test_server_api_create_endpoints());               \
    RUN_TEST(test_server_api_request_single_endpoint());        \
    for (uint32_t i = 16; i <= 512; i *= 2) {                   \
        RUN_TEST(test_server_api_request_many_connections(i));  \
    }                                                           \
    for (size_t i = 16; i <= 16384; i *= 2) {                   \
        RUN_TEST(_test_server_api_echo_random_payload(i));      \
    }

#endif
