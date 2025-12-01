#include "server/compat/adhoc.h"
#include <errno.h>


#ifdef __linux__

#include <sys/epoll.h>

int
compat_epoll_ctl(int epfd,
                 int op,
                 int fd,
                 void *event) {
    return epoll_ctl(epfd, op, fd, event);
}

#else

#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

int
compat_epoll_ctl(int kq,
                 int op,
                 int fd,
                 void *event) {
    struct epoll_event *ev = (struct epoll_event *)event;
    struct kevent kev;

    if (!ev) {
        errno = EINVAL;
        return -1;
    }

    uint32_t events = ev->events;

    int rc = 0;

    switch (op) {

    case EPOLL_CTL_ADD:
        if (events & EPOLLIN) {
            EV_SET(&kev, fd, EVFILT_READ, EV_ADD, 0, 0, ev->data.ptr);
            if (kevent(kq, &kev, 1, NULL, 0, NULL) < 0) return -1;
        }
        if (events & EPOLLOUT) {
            EV_SET(&kev, fd, EVFILT_WRITE, EV_ADD, 0, 0, ev->data.ptr);
            if (kevent(kq, &kev, 1, NULL, 0, NULL) < 0) return -1;
        }
        break;

    case EPOLL_CTL_MOD:
        // In kqueue, modifying means re-adding with EV_ADD | EV_ENABLE
        if (events & EPOLLIN) {
            EV_SET(&kev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, ev->data.ptr);
            if (kevent(kq, &kev, 1, NULL, 0, NULL) < 0) return -1;
        }
        if (events & EPOLLOUT) {
            EV_SET(&kev, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, ev->data.ptr);
            if (kevent(kq, &kev, 1, NULL, 0, NULL) < 0) return -1;
        }
        break;

    case EPOLL_CTL_DEL:
        // Delete both READ and WRITE filters if present
        if (events & EPOLLIN) {
            EV_SET(&kev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            if (kevent(kq, &kev, 1, NULL, 0, NULL) < 0) return -1;
        }
        if (events & EPOLLOUT) {
            EV_SET(&kev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
            if (kevent(kq, &kev, 1, NULL, 0, NULL) < 0) return -1;
        }
        break;

    default:
        errno = EINVAL;
        return -1;
    }

    return rc;
}

#endif
