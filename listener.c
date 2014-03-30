

#include "listener.h"
#include "util.h"
#include "msg.h"

#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>

void listener_init(Listener *sl) {

	int i;

	assert(sl != NULL);

	g_type_init();

	sl->running   = FALSE;

	sl->npfs = 0;
	for (i = 0; i < LISTENER_MAX_SERVERS; i++) {
		sl->pf[i].fd = -1;
		sl->close_callback[i] = NULL;
		sl->opaque[i] = NULL;
	}

	sl->n_clients = 0;
	for (i = 0; i < LISTENER_MAX_CLIENTS; i++) {
		sl->clients[i] = NULL;
	}

	pthread_mutex_init(&sl->clients_lock, NULL);

	sl->queue = queue_create();

}

int listener_add(Listener *sl, int sock) {

	int i;

	pthread_mutex_lock(&sl->clients_lock);

	for (i = 0; i < LISTENER_MAX_SERVERS; i++) {
		if (sl->pf[i].fd == -1) {

			sl->pf[i].fd = sock;
			sl->pf[i].events = POLLIN;
			sl->npfs++;

			i = LISTENER_MAX_SERVERS + 1;
		}
	}

	pthread_mutex_unlock(&sl->clients_lock);

	return (i == (LISTENER_MAX_SERVERS + 1));

}

static void* _client_thread_write(void *arg) {

	int bytes_write;
	Message *front;
	char *txt;

	ClientListener *cl = (ClientListener*) arg;

	while (cl->running_write) {
		// read from in_queue
		// write socket

		DEBUG("Client %d waiting queue", cl->fd);

		front = (Message*) queue_pop(cl->in_queue);

		DEBUG("Client %d: queue front = 0x%x", cl->fd, front);

		if (front != NULL) {

			if (front->type == SIG_QUIT) {
				cl->running_write = FALSE;

				DEBUG("Client %d: writing thread quitting", cl->fd);
			} else {

				DEBUG("Client %d received [%d %d %s]", cl->fd, front->from, front->to, front->msg);

				txt = msg_to_json(front);

				do {
					bytes_write = write(cl->fd, txt, strlen(txt));
				} while (bytes_write == -1 && errno == EAGAIN);

				free(txt);
				msg_destroy(front);

				if (bytes_write == -1) {
					cl->running_write = FALSE;
				} 
			}

		}
	}
}

static void* _client_thread_read(void *arg) {
	int bytes_read;
	char buf[1024];
	struct pollfd pr;

	ClientListener *cl = (ClientListener*) arg;

	pr.fd = cl->fd;
	pr.events = POLLIN;

	while (cl->running_read) {

		DEBUG("Client %d waiting message", cl->fd);

		poll(&pr, 1, 3 * 1000);

		if (pr.revents == POLLIN) {

			do { 
				bytes_read = read(cl->fd, buf, sizeof buf);
			} while (bytes_read == -1 && errno == EAGAIN);

			DEBUG("Client %d sent [%d]", cl->fd, bytes_read);

			if (bytes_read <= 0) {
				cl->running_read = FALSE;

				DEBUG("Client %d: reading thread quitting", cl->fd);
			} else {
				buf[bytes_read] = 0;
				if (bytes_read > 0) {

					//DEBUG("Raw message: %s", buf);

					Message *msgObj = msg_from_json(cl->fd, buf);

					if (msgObj != NULL) {
						DEBUG("Client %d push %d/%d [%d %d %s]", cl->fd, cl->out_queue->size, MAX_SIZE_QUEUE, msgObj->from, msgObj->to, msgObj->msg);
						queue_push(cl->out_queue, (void*) msgObj);
					}
				}
			}

		} else if (pr.revents != 0) {
			cl->running_read = FALSE;
		}
	}

	return NULL;
}

static ClientListener* _client_init(int fd, Queue *out_queue, int fd_server) {

	ClientListener *ret = NULL;

	if (ret = (ClientListener*) malloc(sizeof(ClientListener))) {

		ret->in_queue = queue_create();
		ret->out_queue = out_queue;
		ret->fd = fd;
		ret->fd_server = fd_server;

		ret->running_write = TRUE;
		ret->running_read  = TRUE;

		pthread_create(&ret->read_thread, NULL, _client_thread_read, (void*) ret);
		pthread_create(&ret->write_thread, NULL, _client_thread_write, (void*) ret);
	}

	return ret;
}

static void _client_stop(ClientListener *cl) {

	cl->running_read  = FALSE;
	cl->running_write = FALSE;

	DEBUG("Stopping %d", cl->fd);

	close(cl->fd);

	while (!queue_push(cl->in_queue, (void*) msg_get_quit_msg())) {
		DEBUG("Waiting %d to quit", cl->fd);
		sleep(3);
	}

	DEBUG("CL %d: waiting read thread", cl->fd);
	pthread_join(cl->read_thread, NULL);

	DEBUG("CL %d: waiting write thread", cl->fd);
	pthread_join(cl->write_thread, NULL);

	DEBUG("Client %d down", cl->fd);
}

void listener_close(Listener *l) {
	int i, size;

	DEBUG("Closing all servers [%d]", l->npfs);

	size = l->npfs;
	for (i = 0; i < size; i++) {
		shutdown(l->pf[i].fd, 0);
	}

	queue_push(l->queue, (void*) msg_get_quit_msg());

	DEBUG("Waiting msg processor");
	pthread_join(l->msg_processor, NULL);
	DEBUG("Msg processor ok");

}

static void* __msg_processor(void *arg) {

	Listener *l = (Listener*) arg;
	Message *front, *cpy;
	int running = TRUE;
	int i;

	DEBUG("Initializing msg processor");

	while (l->running && running) {

		front = queue_pop(l->queue);

		if (front != NULL && front->type == SIG_QUIT) {
			running = FALSE;
		} else if (front != NULL) {

			DEBUG("PMSG: [%d %d %s]", front->from, front->to, front->msg);

			pthread_mutex_lock(&l->clients_lock);
			for (i = 0; i < LISTENER_MAX_CLIENTS; i++) {
				if (l->clients[i] != NULL && 
					(l->clients[i]->fd == front->to || front->to == 0)) {
					queue_push(l->clients[i]->in_queue, msg_copy(front));
				}
			}
			pthread_mutex_unlock(&l->clients_lock);

			msg_destroy(front);
		}
	}

	DEBUG("Msg processor stopped");

	return NULL;
}

void _close_server(Listener *sl, int server_index) {

	int j;

	pthread_mutex_lock(&sl->clients_lock);

	for (j = 0; j < LISTENER_MAX_CLIENTS; j++) {
		if (sl->clients[j] != NULL && sl->clients[j]->fd_server == sl->pf[server_index].fd) {
			
			_client_stop(sl->clients[j]);
			
			free(sl->clients[j]);

			sl->clients[j] = NULL;
		}
	}

	sl->pf[server_index].fd = -1;
	sl->npfs--;

	if (sl->close_callback[server_index] != NULL) {
		sl->close_callback[server_index](sl->opaque[server_index]);
	}

	pthread_mutex_unlock(&sl->clients_lock);	

}

void listener_run(Listener *sl) {

	int size, i, j, client;

	sl->running = TRUE;

	pthread_create(&sl->msg_processor, NULL, __msg_processor, (void*) sl);

	while (sl->running) {

 		DEBUG("Listener: Waiting events...");
 		poll(sl->pf, sl->npfs, 3 * 1000);

 		size = sl->npfs;

 		for (i = 0; i < size; i++) {
 			if (sl->pf[i].revents != 0) {
 				if (sl->pf[i].revents != POLLIN) {

 					DEBUG("Server %d down", sl->pf[i].fd);
 					_close_server(sl, i);

 				} else {
 					j = 0;
 					client = accept(sl->pf[i].fd, NULL, NULL);

 					DEBUG("Receiving client %d", client);

 					if (client < 0) {
 						perror("client");
 						j = LISTENER_MAX_CLIENTS + 1;
 					}

 					pthread_mutex_lock(&sl->clients_lock);

 					for (; j < LISTENER_MAX_CLIENTS; j++) {
 						if (sl->clients[j] == NULL) {

 							sl->clients[j] = _client_init(client, sl->queue, sl->pf[i].fd);

 							j = LISTENER_MAX_CLIENTS + 1;
 						}
 					}

 					pthread_mutex_unlock(&sl->clients_lock);
 				} 
 			}
 		}

 		if (size == 0) {
 			sl->running = FALSE;
 		}

 		for (j = 0; j < LISTENER_MAX_CLIENTS; j++) {
 			if (sl->clients[j] != NULL && 
 				(!sl->clients[j]->running_read || !sl->clients[j]->running_write)) {
 				_client_stop(sl->clients[j]);
 				free(sl->clients[j]);
 				sl->clients[j] = NULL;
 			}
 		}

	}

}

void listener_register_close_callback(Listener *l, int fd, void (*callback)(void*), void* argument) {
	int i;
	pthread_mutex_lock(&l->clients_lock);

	for (i = 0; i < LISTENER_MAX_SERVERS; i++) {
		if (l->pf[i].fd == fd) {

			l->close_callback[i] = callback;
			l->opaque[i] = argument;

			i = LISTENER_MAX_SERVERS;
		}
	}

	pthread_mutex_unlock(&l->clients_lock);
}