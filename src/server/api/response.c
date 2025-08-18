#include "server/api/response.h"
#include "server/api/endpoint.h"
#include "server/api/socket.h"
#include "server/str.h"
#include "debug.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>


request_t *_parse_request(char *buff)
{
	char *line, *key, *value;
	uint8_t i = 0;
	request_t *req = (request_t *)malloc(sizeof(request_t));

	if (!req)
		return (NULL);


	req->method = NULL;
	req->target = NULL;
	req->content_type = NULL;
	req->headers = NULL;

	req->body = sstrdup(cut_after_first_delim(buff, "\n\n"));

	line = strtok(buff, "\n");
	while(line)
	{
		key = sstrdup(line);
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

void respond(int32_t client_fd)
{
	char *res, buffer[BUFFER_SIZE];
    request_t *req;
	methods m;
	endpoint_t *e;
	int32_t bytes;

	print_debug("Reached Respond\n");
	bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
	if (bytes > 0)
		buffer[bytes] = '\0';
	else
	{
		close(client_fd);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        return;
	}
	print_debug("received buffer in response: \n%s\n", buffer);
	req = _parse_request(buffer);

	if (!req)
		return;

	m = string_to_method(req->method);

	if (m >= METHODS_COUNT)
		return;

	e = find_endpoint(m, req->target);
	if (!e)
		return;
	res = e->handler(req);
	print_debug("%lu : response below\n", pthread_self());
	print_debug("%lu : %s\n", pthread_self(), res);
	print_debug("%lu : about to send response\n", pthread_self());
	send(client_fd, res, strlen(res), 0);
	print_debug("%lu : just sent response\n", pthread_self());
	if (res)
		free(res);
	free_request(req);
	print_debug("%lu : finished response process\n", pthread_self());
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
	if (req->target)
		free(req->target);
	if (req->headers)
		free(req->headers);
	if (req->content_type)
		free(req->content_type);
	if (req->body)
		free(req->body);
	free(req);
}
