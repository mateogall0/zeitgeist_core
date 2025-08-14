#include "string.h"
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
	char *after, *split;

	if (!src || !delim)
		return (NULL);
	split = strstr(src, delim);
    if (split)
	{
		*split = '\0';
		if (*(split + 2) != '\0')
			after = split + 2;
		return (strdup(after));
	}
	return (NULL);
}
