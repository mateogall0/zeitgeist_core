#include "server/sessions/wheel.h"
#include <time.h>
#include <stdlib.h>


connected_sessions_wheel_t *connected_sessions_wheel = NULL;


connected_sessions_wheel_t *init_connected_sessions_wheel(time_t idle_timout) {
    if (connected_sessions_wheel)
        return (connected_sessions_wheel);

    connected_sessions_wheel = malloc(sizeof(connected_sessions_wheel_t));
    if (!connected_sessions_wheel)
        return (NULL);

    connected_sessions_wheel->head = NULL;
    connected_sessions_wheel->tail = NULL;
    connected_sessions_wheel->idle_timout = idle_timout;

    return (connected_sessions_wheel);
}

connected_session_t *push_new_connected_session_towheel(int client_fd) {
    if (!connected_sessions_wheel)
        return (NULL);

    connected_session_t *new = malloc(sizeof(connected_session_t));
    if (!new)
        return (NULL);

    time_t now = time(NULL);

    new->last_request = now;
    new->start_time = now;
    new->client_fd = client_fd;
    new->next = connected_sessions_wheel->head;
    new->prev = NULL;
    new->meta = (uintptr_t)NULL;

    if (connected_sessions_wheel->head)
        connected_sessions_wheel->head->prev = new;
    if (!connected_sessions_wheel->tail)
        connected_sessions_wheel->tail = new;

    connected_sessions_wheel->head = new;

    return (new);
}

connected_session_t *pop_last_connected_session_fromwheel() {
    if (!connected_sessions_wheel || !connected_sessions_wheel->tail)
        return (NULL);

    connected_session_t *popped = connected_sessions_wheel->tail;
    connected_sessions_wheel->tail = popped->prev;
    if (popped->prev) {
        popped->prev->next = NULL;
        popped->prev = NULL;
    }
    else {
        connected_sessions_wheel->head = NULL;
    }
    return (popped);
}

connected_session_t *pop_selected_connected_session_fromwheel(connected_session_t *session) {
    if (!connected_sessions_wheel ||
        !connected_sessions_wheel->tail ||
        !session)
        return (NULL);
    if (session->next)
        session->next->prev = session->prev;
    else
        connected_sessions_wheel->tail = session->prev;
    if (session->prev)
        session->prev->next = session->next;
    else
        connected_sessions_wheel->head = session->next;

    session->prev = NULL;
    session->next = NULL;
    return (session);
}



void destroy_connected_wheel_session(connected_session_t *session) {
    if (session)
        free(session);
}

void destroy_connected_sessions_wheel() {
    if (!connected_sessions_wheel)
        return;
    connected_session_t *current = pop_last_connected_session_fromwheel();
    while(current) {
        destroy_connected_wheel_session(current);
        current = pop_last_connected_session_fromwheel();
    }
    free(connected_sessions_wheel);
    connected_sessions_wheel = NULL;
}

connected_session_t *peek_last_connected_wheel_session() {
    if (!connected_sessions_wheel)
        return (NULL);
    return (connected_sessions_wheel->tail);
}
