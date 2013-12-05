
#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <poll.h>

typedef struct _Client {

	int sockfd;
	struct pollfd pd;

} Client;

Client *client_init(const char *address, int port);
void client_main_loop(Client *client);

#endif
