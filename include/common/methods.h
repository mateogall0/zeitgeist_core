#ifndef ZCOMMON_METHODS_H
#define ZCOMMON_METHODS_H


#ifdef __cplusplus
extern "C" {
#endif


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


#ifdef __cplusplus
}
#endif

#endif
