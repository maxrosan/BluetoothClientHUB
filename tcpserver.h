
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/poll.h>
#include <pthread.h>

typedef struct _Server {

	int sock;
	int running;

	struct pollfd fds[1024];
	int nfds;

	pthread_t thread;

} TServer;

 int server_init(TServer *server, int port);
 void server_start(TServer *server);

 #endif