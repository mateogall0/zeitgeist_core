#include "client/unresolved.h"
#include <stdlib.h>


unresolved_requests_list_t *
create_unresolved_requests_list() {
    unresolved_requests_list_t *new;

    new = malloc(sizeof(unresolved_requests_list_t));
    if (!new)
        return (NULL);
    new->size = 0;
    new->head = NULL;

    return (new);
}

unresolved_request_t *
remove_unresolved_request_from_list(unresolved_requests_list_t *list,
                                    unsigned long id) {
    if (!list)
        return (NULL);

    unresolved_request_t *found;
    found = find_unresolved_request_from_list(list, id);

    if (!found)
        return (NULL);

    if (found->next)
        found->next->prev = found->prev;

    if (found->prev)
        found->prev->next = found->next;

    if (found == list->head)
        list->head = found->next;

    found->next = NULL;
    found->prev = NULL;

    --list->size;

    return (found);
}

unresolved_request_t *
push_unresolved_request_from_list(unresolved_requests_list_t *list,
                                  unsigned long id) {
    if (!list ||
        id == 0 ||
        find_unresolved_request_from_list(list, id))
        return (NULL);

    unresolved_request_t *new = malloc(sizeof(unresolved_request_t));
    if (!new)
        return (NULL);

    if (list->head)
        list->head->prev = new;

    new->next = list->head;
    list->head = new;
    new->prev = NULL;
    new->id = id;

    ++list->size;

    return (new);
}

unresolved_request_t *
find_unresolved_request_from_list(unresolved_requests_list_t *list,
                                  unsigned long id) {
    if (!list || id == 0)
        return (NULL);

    unresolved_request_t *current = list->head;
    while (current) {
        if (current->id == id)
            break;
        current = current->next;
    }

    return (current);
}

void
free_unresolved_request(unresolved_request_t *req) {
    if (req)
        free(req);
}

void
free_unresolved_requests_list(unresolved_requests_list_t *list) {
    if (!list)
        return;

    unresolved_request_t *current = list->head, *tmp;
    while (current) {
        tmp = current->next;
        free(current);
        current = tmp;
    }

    free(list);
}
