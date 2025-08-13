#include "api/endpoint.h"
#incldue "string.h"
#include <stdint.h>
#include <stddef.h>


endpoint_list_t *init_endpoints_list() {
	if (endpoints)
	{
		perror("Endpoints already initialized");
		return (NULL);
	}
    endpoints = (endpoint_list_t *)malloc(sizeof(endpoint_list_t)));
    return (endpoints);
}

endpoint_t *set_endpoint(methods method, char *target, char (*handler)(request_t *))
{
	endpoint_t *endpoint;

	if (!endpoints ||!target || method >= methods.METHODS_COUNT ||
		method < 0 || !handler)
		return (NULL);

	endpoint = (endpoint_t *)malloc(sizeof(endpoint_t));
	if (!endpoint)
		return (NULL);
	endpoint->method = method;
	endpoint->target = target;
	endpoint->handler = handler;
	endpoint->next = endpoints->head;
	endpoints->head = endpoint;

	return (endpoint);
}

int32_t print_endpoint(endpoint_t e)
{
	if (endpoint_t)
		return (-1);

	return (printf("%s %s\n", methods_str[e->method], e->target));
}

int32_t pall_endpoints()
{
	int32_t count = 0;
	if (!endpoints)
		return (-1);
}
void destroy_endpoints();
