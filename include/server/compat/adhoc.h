#ifndef ZSERVER_COMPAT_ADHOC_H
#define ZSERVER_COMPAT_ADHOC_H

#ifdef __linux__

#include <sys/epoll.h>

#else  // macOS / BSD

#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

#define EPOLLIN     0x001
#define EPOLLOUT    0x004

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

struct epoll_event {
    uint32_t events;
    union {
        void *ptr;
        int fd;
        uint32_t u32;
        uint64_t u64;
    } data;
};

#endif

int
compat_epoll_ctl(int epfd, int op, int fd,
                 void *event);

#endif
