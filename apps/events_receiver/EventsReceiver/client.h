
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
#include <glib.h>
#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>

#include <X11/keysymdef.h>

enum KEY_TYPE { INVALID, ACCELEROMETER, VOLUME_UP, VOLUME_DOWN };

typedef struct SMessage {

	int from, to;
	float x, y, z;
	int type;

} Message;

typedef struct _Client {

	int sockfd;
	struct pollfd pd;
	char buffer[4096];
	int running;

} Client;

Client *client_init(const char *address, int port);
void client_main_loop(Client *client, void (*callback)(Message *msg));

#endif
