#ifndef ZSERVER_SESSIONS_WHEEL_H
#define ZSERVER_SESSIONS_WHEEL_H


#include <time.h>


typedef struct connected_session_s {
    int client_fd;
    time_t last_request;
    time_t start_time;
    struct connected_session_s *prev;
    struct connected_session_s *next;
} connected_session_t;

typedef struct {
    time_t idle_timout;
    connected_session_t *head;
    connected_session_t *tail;
} connected_sessions_wheel_t;


extern connected_sessions_wheel_t *connected_sessions_wheel;

connected_sessions_wheel_t *init_connected_sessions_wheel(time_t idle_timout);
connected_session_t *push_new_connected_session_towheel(int client_fd);
connected_session_t *pop_last_connected_session_fromwheel();
connected_session_t *pop_selected_connected_session_fromwheel(connected_session_t *session);
void destroy_connected_wheel_session(connected_session_t *session);
void destroy_connected_sessions_wheel();
connected_session_t *peek_last_connected_wheel_session();

#endif
