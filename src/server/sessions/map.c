#include "server/sessions/map.h"
#include <stdlib.h>
#include <stddef.h>


connected_sessions_map_t *connected_sessions_map = NULL;


connected_sessions_map_t *init_connected_sessions_map(size_t size) {
    if (connected_sessions_map)
        return (connected_sessions_map);
    if (size == 0)
        return (NULL);

    connected_sessions_map = malloc(sizeof(connected_sessions_map_t));
    if (!connected_sessions_map)
        return (NULL);
    connected_sessions_map->_map = calloc(size, sizeof(connected_session_t *));
    if (!connected_sessions_map->_map) {
        free(connected_sessions_map);
        return (NULL);
    }
    connected_sessions_map->size = size;
    return(connected_sessions_map);
}

connected_session_t *add_connected_session(int fd) {
    if (!connected_sessions_map ||
        !connected_sessions_map ||
        (size_t)fd >= connected_sessions_map->size)
        return (NULL);

    // check if it already exists, if so destroy it
    // it's an edge case that is likely to never hit
    // in the loop
    if (get_connected_session(fd))
        destroy_connected_session(fd);

    connected_session_t *new = push_new_connected_session_towheel(fd);
    if (!new)
        return (NULL);
    connected_sessions_map->_map[fd] = new;
    return (new);
}

connected_session_t *get_connected_session(int fd) {
    if (!connected_sessions_map ||
        !connected_sessions_map->_map ||
        (size_t)fd >= connected_sessions_map->size)
        return (NULL);

    return (connected_sessions_map->_map[fd]);
}

void destroy_connected_session(int fd) {
    if (!connected_sessions_map ||
        !connected_sessions_map->_map ||
        (size_t)fd >= connected_sessions_map->size)
        return;
    connected_session_t *s = pop_selected_connected_session_fromwheel(get_connected_session(fd));
    destroy_connected_wheel_session(s);
    connected_sessions_map->_map[fd] = NULL;
}

void destroy_connected_sessions_map() {
    if (!connected_sessions_map ||
        !connected_sessions_map->_map)
        return;
    free(connected_sessions_map->_map);
    free(connected_sessions_map);
    connected_sessions_map = NULL;
}
