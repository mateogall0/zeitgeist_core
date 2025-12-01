#ifndef STRING_H
#define STRING_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>


char *strdup(const char *src);
int8_t string_in_array(const char *str, const char *arr[], size_t size);
char *cut_after_first_delim(const char *src, const char *delim);
char *sstrdup(char *src); // safe strdup

#ifdef __cplusplus
}
#endif

#endif
