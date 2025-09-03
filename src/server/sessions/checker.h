#include "server/sessions/wheel.h"
#include "server/sessions/map.h"
#include <time.h>



void clear_idle_connection_sessions() {
    if (!connected_sessions_wheel)
        return;
    connected_session_t *last;
    time_t now = time(NULL);
    while(last = peek_last_connected_wheel_session()) {
        if (now > last->last_request + connected_sessions_wheel->idle_timout) {
        }
        else
            break;
    }
}
