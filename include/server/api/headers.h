#ifndef ZSERVER_API_HEADERS_H
#define ZSERVER_API_HEADERS_H


typedef struct request_header_s {
    char *key;
    char *value;
    struct request_header_s *next;
} request_header_t;

typedef struct {
    request_header_t *head;
} request_headers_list_t;

request_headers_list_t *create_headers_list();
request_header_t *create_push_header_to_list(request_headers_list_t *list, char *key, char *value);
void destroy_headers_list(request_headers_list_t *list);

#endif
