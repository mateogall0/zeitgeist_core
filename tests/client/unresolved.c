#include "tests.h"
#include "client/unresolved.h"


int8_t test_client_unresolved_creation() {

    unresolved_requests_list_t *l = create_unresolved_requests_list();

    ASSERT(l);
    ASSERT(l->size == 0);
    ASSERT(!l->head);

    for (unsigned long i = 1; i <= 128; ++i) {
        ASSERT(push_unresolved_request_from_list(l, i));
        ASSERT(l->size == i);
    }

    free_unresolved_requests_list(l);

    return (0);
}

int8_t test_client_unresolved_pop() {
    unresolved_requests_list_t *l = create_unresolved_requests_list();
    ASSERT(l);

    for (unsigned long i = 1; i <= 1024; ++i) {
        ASSERT(push_unresolved_request_from_list(l, i));
    }
    ASSERT(l->size == 1024);

    unresolved_request_t *req = remove_unresolved_request_from_list(l, 567);
    ASSERT(req);
    ASSERT(req->id == 567);
    ASSERT(l->size == 1023);
    free_unresolved_request(req);

    req = remove_unresolved_request_from_list(l, 5);
    ASSERT(req);
    ASSERT(req->id == 5);
    ASSERT(l->size == 1022);
    free_unresolved_request(req);


    req = remove_unresolved_request_from_list(l, 1);
    ASSERT(req);
    ASSERT(req->id == 1);
    ASSERT(l->size == 1021);
    free_unresolved_request(req);

    req = remove_unresolved_request_from_list(l, 1021);
    ASSERT(req);
    ASSERT(req->id == 1021);
    ASSERT(l->size == 1020);
    free_unresolved_request(req);


    free_unresolved_requests_list(l);

    return (0);
}
