#include "tests.h"
#include "server/sessions/map.h"
#include "server/sessions/wheel.h"

int8_t test_server_sessions_map_init_zerosize() {
    ASSERT(!connected_sessions_map);
    ASSERT(!init_connected_sessions_map(0));
    ASSERT(!connected_sessions_map);
    return (0);
}

int8_t _test_server_sessions_map_push(size_t map_size) {
    ASSERT(!connected_sessions_map);
    ASSERT(init_connected_sessions_map(map_size));
    ASSERT(connected_sessions_map);
    init_connected_sessions_wheel(1024);

    size_t i;
    for (i = 0; i < map_size; ++i) {
        ASSERT(add_connected_session(i));
    }
    ASSERT(!add_connected_session(i));

    i = 0;
    connected_session_t *current = connected_sessions_wheel->head;
    while (current) {
        current = current->next;
        i++;
    }
    ASSERT(i == map_size);
    destroy_connected_sessions_wheel();
    destroy_connected_sessions_map();
    ASSERT(!connected_sessions_map);
    return (0);
}
