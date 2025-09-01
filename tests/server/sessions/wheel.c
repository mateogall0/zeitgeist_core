#include "server/sessions/wheel.h"
#include "tests.h"
#include <stdint.h>


int8_t test_server_sessions_wheel_push() {
    ASSERT(!connected_sessions_wheel);
    init_connected_sessions_wheel(64);
    ASSERT(connected_sessions_wheel);
    int i, max = 1024;
    for (i = 1; i <= max; ++i)
      ASSERT(push_new_connected_session_towheel(i));

    connected_session_t *current = connected_sessions_wheel->head;
    ASSERT(current);
    i = max;
    while (current) {
        ASSERT(i-- == current->client_fd);
        current = current->next;
    }

    current = connected_sessions_wheel->tail;
    i = 1;
    while (current) {
        ASSERT(i++ == current->client_fd);
        current = current->prev;
    }
    ASSERT(i == max + 1);

    destroy_connected_sessions_wheel();
    ASSERT(!connected_sessions_wheel);
    return(0);
}
