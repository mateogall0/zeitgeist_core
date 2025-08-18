#include "server/api/endpoint.h"
#include "server/str.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


const char *methods_str[METHODS_COUNT] = {
    "GET",
    "PUT",
    "DELETE",
    "POST"
};

endpoint_list_t *endpoints = NULL;

endpoint_list_t *init_endpoints_list() {
	if (endpoints)
	{
		perror("Endpoints already initialized");
		return (endpoints);
	}
    endpoints = (endpoint_list_t *)malloc(sizeof(endpoint_list_t));
	endpoints->head = NULL;
    return (endpoints);
}

endpoint_t *set_endpoint(methods method, char *target, char *(*handler)(request_t *))
{
	endpoint_t *endpoint;

	if (!endpoints ||!target || method >= METHODS_COUNT ||
		method < 0 || !handler)
		return (NULL);

	endpoint = (endpoint_t *)malloc(sizeof(endpoint_t));
	if (!endpoint)
		return (NULL);
	endpoint->method = method;
	endpoint->target = strdup(target);
	endpoint->handler = handler;
	endpoint->next = endpoints->head;
	endpoints->head = endpoint;

	return (endpoint);
}

int32_t print_endpoint(endpoint_t *e)
{
	if (!e)
		return (-1);

	return (printf("%s %s\n", methods_str[e->method], e->target));
}

int32_t pall_endpoints()
{
	int32_t count = 0;
	endpoint_t *current;

	if (!endpoints)
		return (-1);
	current = endpoints->head;
	while(current)
	{
		count += print_endpoint(current);
		current = current->next;
	}
	return (count);
}

void destroy_endpoints()
{
	endpoint_t *current;
	if (!endpoints)
		return;
	while(endpoints->head)
	{
		current = endpoints->head;
		endpoints->head = endpoints->head->next;
		free(current->target);
		free(current);
	}
	free(endpoints);
	endpoints = NULL;
}

endpoint_t *find_endpoint(methods method, char *target)
{
	endpoint_t *current;

	if (!endpoints || method < 0 || method >= METHODS_COUNT || !target)
		return (NULL);

	current = endpoints->head;

	while (current)
	{
		if (current->method == method && strcmp(target, current->target) == 0)
			return (current);
	}
	return (NULL);
}

methods string_to_method(const char *str)
{
	int32_t i;

	if(!str)
		return (METHODS_COUNT);

    for (i = 0; i < METHODS_COUNT; i++)
	{
        if (strcmp(str, methods_str[i]) == 0)
            return ((methods)i);
    }
    return (METHODS_COUNT);
}
