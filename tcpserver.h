
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/poll.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "msg.h"

#define MAX_CLIENTS 20
#define TCP_MAX_BUFF_LEN 1024

typedef struct _Server {

	int sock;
	int running;

	struct pollfd fds[1 + MAX_CLIENTS];
	int nfds;

	pthread_t thread;
	pthread_mutex_t lock;

	struct _Client *clients[MAX_CLIENTS];
	int            n_clients;

} TServer;

typedef struct _Client {

	int sock;
	int running;

	Queue *queue;

	pthread_t thread;

} TClient;

 int server_init(TServer *server, int port);
 void server_start(TServer *server);
 void server_send_msg(TServer *server, Message *msg);

 #endif
