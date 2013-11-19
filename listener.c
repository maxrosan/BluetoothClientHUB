

#include "listener.h"
#include "util.h"

#include <unistd.h>
#include <assert.h>

void listener_init(Listener *sl) {

	int i;

	assert(sl != NULL);

	sl->running   = FALSE;

	sl->npfs = 0;
	for (i = 0; i < LISTENER_MAX_SERVERS; i++) {
		sl->pf[i].fd = -1;
	}

	sl->n_clients = 0;
	for (i = 0; i < LISTENER_MAX_CLIENTS; i++) {
		sl->clients[i] = NULL;
	}

}

void listener_add(Listener *sl, int sock) {

	int i;

	for (i = 0; i < LISTENER_MAX_SERVERS; i++) {
		if (sl->pf[i].fd == -1) {

			sl->pf[i].fd = sock;
			sl->pf[i].events = POLLIN;

			i = LISTENER_MAX_SERVERS + 1;
		}
	}

	return (i == (LISTENER_MAX_SERVERS + 1));

}

static void* _client_thread_write(void *arg) {

	int bytes_write;
	char buf[1024];

	ClientListener *cl = (ClientListener*) arg;

	while (cl->running) {
		// read from in_queue
		// write socket
	}
}

static void* _client_thread_read(void *arg) {
	int bytes_read;
	char buf[1024];

	ClientListener *cl = (ClientListener*) arg;

	while (cl->running) {

		do { 
			bytes_read = read(client, buf, sizeof buf);
		} while (bytes_read == -1 && errno == EAGAIN);
		
		// put it into out_queue

		if (bytes_read == -1) {
			cl->running = FALSE;
		}
	}

	return NULL;
}

static ClientListener* void _client_init(int fd, void *id, Queue *out_queue, int fd_server) {

	ClientListener *ret = NULL;

	if (ret = (ClientListener*) malloc(sizeof(ClientListener))) {
		ret->id = id;
		ret->in_queue = queue_create();
		ret->out_queue = out_queue;
		ret->fd = fd;
		ret->fd_server = fd_server;
		ret->running = TRUE;

		pthread_create(&ret->read_thread, NULL, _client_thread_read, (void*) ret);
		pthread_create(&ret->write_thread, NULL, _client_thread_write, (void*) ret);
	}

	return ret;
}

void listener_run(Listener *sl) {

	int size, i, j;

	sl->running = TRUE;

	while (sl->running) {

 		fprintf(stderr, "Listener: Waiting events...\n");
 		poll(sl->pf, server->npfs, 3 * 1000);

 		size = sl->npfs;
 		for (i = 0; i < size; i++) {
 			if (sl->pf[i].revents != 0) {
 				if (sl->pfs[i].revents != POLLIN) {
 					//sl->pfs[i].fd = -1;
 				} else {
 					client = accept(sl->pfs[i].fd, NULL, NULL);
 				} 
 			}
 		}

	}

}