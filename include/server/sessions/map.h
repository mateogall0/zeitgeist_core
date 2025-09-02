#ifndef ZSERVER_SESSIONS_MAP_H
#define ZSERVER_SESSIONS_MAP_H

#include "server/sessions/wheel.h"
#include <stddef.h>


typedef struct {
    connected_session_t **_map;
    size_t size;
} connected_sessions_map_t;

extern connected_sessions_map_t *connected_sessions_map;

connected_sessions_map_t *init_connected_sessions_map(size_t size);
connected_session_t *add_connected_session(int fd);
connected_session_t *get_connected_session(int fd);
void destroy_connected_session(int fd);
void destroy_connected_sessions_map();


#endif
