
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/poll.h>
#include <pthread.h>
#include <errno.h>

#define MAX_QUEUE 1024
#define MAX_CLIENTS 20

typedef struct _Server {

	int sock;
	int running;

	struct pollfd fds[1];
	int nfds;

	pthread_t thread;
	pthread_mutex_t lock;

	struct _Client *clients[MAX_CLIENTS];
	int            n_clients;

} TServer;

typedef struct _Client {

	int sock;
	int running;

	char* queue[MAX_QUEUE];
	int   queue_front;
	int   queue_rear;
	int   queue_size;

	pthread_t thread;
	pthread_mutex_t lock;
	pthread_cond_t  cond;

} TClient;

 int server_init(TServer *server, int port);
 void server_start(TServer *server);
 void server_send_msg(TServer *server, char *msg);

 #endif
