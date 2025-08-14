#include "api/response.h"
#include "api/endpoint.h"
#include "str.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


request_t *_parse_request(char *buff)
{
	char *line, *key, *value;
	uint8_t i = 0;
	request_t *req = (request_t *)malloc(sizeof(request_t));

	if (!req)
		return (NULL);

	req->body = sstrdup(cut_after_first_delim(buff, "\n\n"));

	line = strtok(buff, "\n");
	while(line)
	{
		key = sstrdup(line);
		printf("key: %s$\n", key);
		switch (i) {
		case METHOD:
			value = cut_after_first_delim(key, " ");
			if (!string_in_array(key, methods_str, METHODS_COUNT))
			{
				free_request(req);
				return (NULL);
			}
			req->method = sstrdup(key);
			req->target = sstrdup(value);
			break;
		case CONTENT_TYPE:
			value = cut_after_first_delim(key, ": ");
			if (strcmp(key, "Content-Type") != 0)
			{
				free_request(req);
				free(key);
				return (NULL);
			}
			req->content_type = sstrdup(value);
			break;
		case HEADERS:
			value = cut_after_first_delim(key, ": ");
			if (strcmp(key, "Headers") != 0)
			{
				free_request(req);
				free(key);
				return (NULL);
			}
			req->headers = sstrdup(value);
			break;
		default:
			free_request(req);
			free(key);
			return (NULL);
		}
		free(key);
		i++;
		line = strtok(NULL, "\n");
	}
	return (req);
}

void respond(int32_t client_fd, char *buffer)
{
	char *res;
    request_t *req = _parse_request(buffer);
	methods m;
	endpoint_t *e;
	if (!req)
		return;

	m = string_to_method(req->method);

	if (m >= METHODS_COUNT)
		return;

	e = find_endpoint(m, req->target);
	if (!e)
		return;
	res = e->handler(req);
	send(client_fd, res, strlen(res), 0);
}


int32_t print_request(request_t *req)
{
	if (!req)
		return (-1);

	return printf("%s\n%s\n%s\n%s\n",
				  req->method,
				  req->content_type,
				  req->headers,
				  req->body);
}

void free_request(request_t *req)
{
	if (!req)
		return;
	if (req->method)
		free(req->method);
	if (req->headers)
		free(req->headers);
	if (req->content_type)
		free(req->content_type);
	if (req->body)
		free(req->body);
	free(req);
}
