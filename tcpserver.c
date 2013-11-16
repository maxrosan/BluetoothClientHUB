

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

#include "tcpserver.h"
#include "util.h"

 int server_init(TServer *server, int port) {

 	assert(server != NULL);
 	assert(port > 0);

	struct sockaddr_in servaddr;
	int on = 1;

 	server->sock = socket(AF_INET, SOCK_STREAM, 0);

 	ioctl(server->sock, FIONBIO, (char*) &on, sizeof(on));

	memset((void*) &servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if (bind(server->sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
		perror("server_init");
		close(server->sock);

		return FALSE;
	}

	if (listen(server->sock, 1024) == -1) {
		perror("server_init");
		close(server->sock);

		return FALSE;
	}

	server->running = TRUE;

	return TRUE;
 }

 int _client_queue_push(TClient *client, char *str) {

 	assert(client != NULL);
 	assert(str != NULL);

 	if (((client->queue_front + 1) % MAX_QUEUE) != (client->queue_rear)) {

 		client->queue[client->queue_front] = strdup(str);
 		client->queue_front = (client->queue_front + 1) % MAX_QUEUE;

 		fprintf(stderr, "Message to %d pushed\n", client->sock);

 		return TRUE;
 	} else {
		fprintf(stderr, "Message to %d not pushed\n", client->sock); 		
 	}

 	return FALSE;
 }

 static int _client_queue_empty(TClient *client) {

 	assert(client != NULL);

 	return (client->queue_rear == client->queue_front);

 }

 static int _client_send_msg(TClient *client, char *msg) {

 	int res, sendsig;
 	printf("Sending [%s] to %d\n", msg, client->sock);

 	pthread_mutex_lock(&client->lock);
 	sendsig = _client_queue_empty(client);
 	res = _client_queue_push(client, msg);

 	if (res && sendsig) {
 		pthread_cond_signal(&client->cond);
 	}

 	pthread_mutex_unlock(&client->lock);

 	return res;
 }

 static char* _client_queue_pop(TClient *client) {

 	assert(client != NULL);

 	if (client->queue_rear == client->queue_front) {
 		return NULL;
 	} else {

 		char *front = client->queue[client->queue_rear];
 		char *res = strdup(front);

 		client->queue[client->queue_rear] = NULL;
 		client->queue_rear = (client->queue_rear + 1) % MAX_QUEUE;

 		free(front);

 		return res;
 	}
 }

 static void* _client_run(void *arg) {

 	assert(arg != NULL);

 	TClient *client = (TClient*) arg;
 	char *front;

 	client->running = TRUE;

 	fprintf(stderr, "Client %d: running\n", client->sock);

 	while (client->running) {

 		fprintf(stderr, "Client %d: waiting message\n", client->sock);

 		pthread_mutex_lock(&client->lock);

 		if (_client_queue_empty(client)) {
 			pthread_cond_wait(&client->cond, &client->lock);
 		}


 		front = _client_queue_pop(client);

 		if (strcmp("QUIT", front) == 0) {
 			client->running = FALSE;
 		} else {
 			int try_write;

 			do {
 				try_write =  write(client->sock, front, strlen(front) + 1);
 			} while (try_write < 0 && errno == EAGAIN);

 			if (try_write == -1) {
 				client->running = FALSE;
 			}
 		}

 		free(front);

 		pthread_mutex_unlock(&client->lock);

 	}

 	return NULL;
 }

 static TClient *_client_thread_init(int sock) {

	TClient *client = (TClient*) malloc(sizeof(TClient));

	if (client != NULL) {
		client->sock = sock;
		client->running = FALSE;
		client->queue_front = 0;
		client->queue_rear  = 0;
	
		memset(client->queue, 0, sizeof(char*) * MAX_QUEUE);

		pthread_mutex_init(&client->lock, NULL);
 		pthread_cond_init(&client->cond, NULL);

		pthread_create(&client->thread, NULL, _client_run, client);

 	}

	return client;
 }

 static void _client_destroy(TClient *client) {

 	pthread_cond_destroy(&client->cond);
 	pthread_mutex_destroy(&client->lock);

	free(client);
 }

 static void *_server_thread_run(void *arg) {

 	TServer *server = (TServer*) arg;
 	int i, size;
 	char buff[250];
 	int bufflen;

 	assert(server != NULL);

 	server->nfds = 1;

	server->fds[0].fd = server->sock;

	server->fds[0].events = POLLIN;

 	while (server->running) {

 		fprintf(stderr, "Server: Waiting events...\n");
 		poll(server->fds, server->nfds, 3 * 1000);

 		size = server->nfds;
 		for (i = 0; i < size; i++) {
 			if (server->fds[i].revents != 0) {
 				if (server->fds[i].revents != POLLIN) {
 					server->running = FALSE;
 				} else if (server->fds[i].fd == server->sock) {
 					int newfd;

 					do {

 						newfd = accept(server->sock, NULL, NULL);

 						if (newfd < 0) {
 							if (errno != EWOULDBLOCK) {
 								perror("accept() failed");
 								server->running = FALSE;
 							}
 						} else {

 							pthread_mutex_lock(&server->lock);

							if (server->n_clients < MAX_CLIENTS) {
								int j;
								for (j = 0; j < MAX_CLIENTS; j++) {
									if (server->clients[i] == NULL) {

										if (server->clients[i] = _client_thread_init(newfd)) {
											server->n_clients++;
											fprintf(stderr, "Server: Client added in %d...\n", i);
										}

										i = MAX_CLIENTS + 1; // break
									}
								}
							}

							pthread_mutex_unlock(&server->lock);
 						}

 					} while (newfd != -1);

 				}

 			}
 		}

 		pthread_mutex_lock(&server->lock);
 		for (i = 0; i < MAX_CLIENTS; i++) {
 			if (server->clients[i] != NULL && !server->clients[i]->running) {
 				pthread_join(server->clients[i]->thread, NULL);

 				_client_destroy(server->clients[i]);
 				server->clients[i] = NULL;
 				server->n_clients--;
 			}
 		}
 		pthread_mutex_unlock(&server->lock);

 	}


 	close(server->sock);

	return 0;
 }

 void server_start(TServer *server) {

 	assert(server != NULL);

 	pthread_mutex_init(&server->lock, NULL);
 	pthread_create(&server->thread, NULL, _server_thread_run, (void*) server);

 }

 void server_send_msg(TServer *server, char *msg) {

 	int i;

 	assert(server != NULL && msg != NULL);

 	fprintf(stderr, "Waiting to send message to all %d clients\n", server->n_clients);
 	pthread_mutex_lock(&server->lock);

 	for (i = 0; i < MAX_CLIENTS; i++) {
 		if (server->clients[i] != NULL) {

 			fprintf(stderr, "Sending message to %d\n", i);

 			_client_send_msg(server->clients[i], msg);
 		}
 	}

 	pthread_mutex_unlock(&server->lock);
	fprintf(stderr, "Message sent to all clients\n"); 		
 }