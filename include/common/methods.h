#ifndef ZCOMMON_METHODS_H
#define ZCOMMON_METHODS_H


typedef enum {
    GET,
    PUT,
    DELETE,
    POST,
    HEAD,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
    METHODS_COUNT,
} methods;

extern const char *methods_str[METHODS_COUNT];


methods string_to_method(const char *str);


#endif
