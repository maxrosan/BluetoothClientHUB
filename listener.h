
#ifndef LISTENER_H
#define LISTENER_H

#define LISTENER_MAX_SERVERS 32
#define LISTENER_MAX_CLIENTS 1024

#include <pthread.h>

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#include "queue.h"

#define VERIFY_MSG_FUNC(NAME) int (*NAME)(ClientListener *cl, Message *msg)

typedef struct _ClientListener {

	Queue *in_queue;
	Queue *out_queue;

	struct _Listener *listener;
	int fd_server;

	int fd;

	pthread_t read_thread;
	pthread_t write_thread;
	int running_read, running_write;

} ClientListener;


typedef struct _Listener {

	int running;

	struct pollfd pf[LISTENER_MAX_SERVERS];
	int           npfs;

	ClientListener *clients[LISTENER_MAX_CLIENTS];
	int            n_clients;

	pthread_mutex_t clients_lock;

	Queue *queue;

	pthread_t msg_processor;

} Listener;

void listener_init(Listener *sl);
int listener_add(Listener *sl, int sock);
void listener_run(Listener *sl);
void listener_close(Listener *l);

#endif