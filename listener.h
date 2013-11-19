
#ifndef LISTENER_H
#define LISTENER_H

#define LISTENER_MAX_SERVERS 32
#define LISTENER_MAX_CLIENTS 1024

#include <pthread.h>

#include "queue.h"

#define VERIFY_MSG_FUNC(NAME) int (*NAME)(ClientListener *cl, Message *msg)

typedef struct _ClientListener {

	void *id;
	Queue *in_queue;
	Queue *out_queue;

	struct _Listener *listener;
	int fd_server;

	int fd;

	pthread_t read_thread;
	pthread_t write_thread;
	int running;

} ClientListener;


typedef struct _Listener {

	int running;

	struct pollfd pf[LISTENER_MAX_SERVERS];
	int           npfs;

	ClientListener *clients[LISTENER_MAX_CLIENTS];
	int            n_clients;

	pthread_t msg_processor_thread;

} Listener;

void listener_init(Listener *sl);
void listener_add(Listener *sl, int sock);
int listener_run(Listener *sl);

#endif