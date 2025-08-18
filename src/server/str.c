#include "server/str.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>


int8_t string_in_array(const char *str, const char *arr[], size_t size)
{
	if (!str || !arr)
		return (0);
    for (size_t i = 0; i < size; i++)
	{
        if (strcmp(str, arr[i]) == 0)
            return (1);
    }
    return (0);
}

char *cut_after_first_delim(const char *src, const char *delim)
{
	char *split;
    char *after = (NULL);

    if (!src || !delim)
        return (NULL);

    split = strstr(src, delim);
    if (!split)
        return (NULL);

    *split = '\0';
    split += strlen(delim);

    if (*split != '\0')
        after = split;

    if (after)
        return after;

    return (NULL);
}

char *sstrdup(char *src)
{
	if (!src)
		return (NULL);
	return (strdup(src));
}
