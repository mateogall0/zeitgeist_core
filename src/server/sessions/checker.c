#include "server/sessions/wheel.h"
#include "server/sessions/map.h"
#include <time.h>
#include <unistd.h>
#include "server/compat/adhoc.h"

void clear_idle_connection_sessions(int epoll_fd) {
    if (!connected_sessions_wheel)
        return;
    connected_session_t *last;
    while((last = peek_last_connected_wheel_session())) {
        if (time(NULL) > last->last_request + connected_sessions_wheel->idle_timout) {
            int client_fd = last->client_fd;
            destroy_connected_session(client_fd);
            if (compat_epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == 0) {
                close(client_fd);
                destroy_connected_session(client_fd);
            }
        }
        else
            break;
    }
}
