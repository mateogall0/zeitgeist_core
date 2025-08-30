#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *random_string(size_t length) {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    size_t charset_size = sizeof(charset) - 1;

    char *str = malloc(length + 1);
    if (!str) return NULL;

    for (size_t i = 0; i < length; i++) {
        int key = rand() % charset_size;
        str[i] = charset[key];
    }
    str[length] = '\0';
    return str;
}
