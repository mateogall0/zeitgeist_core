#ifndef ZCLIENT_UNRESOLVED_H
#define ZCLIENT_UNRESOLVED_H


#include <stddef.h>


typedef struct unresolved_request_s {
    struct unresolved_request_s *next;
    struct unresolved_request_s *prev;
    unsigned long id;
} unresolved_request_t;


typedef struct {
    unresolved_request_t *head;
    size_t size;
} unresolved_requests_list_t;


unresolved_requests_list_t *
create_unresolved_requests_list();

unresolved_request_t *
remove_unresolved_request_from_list(unresolved_requests_list_t *list,
                                    unsigned long id);

unresolved_request_t *
push_unresolved_request_from_list(unresolved_requests_list_t *list,
                                  unsigned long id);

unresolved_request_t *
find_unresolved_request_from_list(unresolved_requests_list_t *list,
                                  unsigned long id);

void
free_unresolved_request(unresolved_request_t *req);

void
free_unresolved_requests_list(unresolved_requests_list_t *list);


#endif
