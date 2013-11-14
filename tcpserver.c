

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

 void *_server_thread_run(void *arg) {

 	TServer *server = (TServer*) arg;
 	int i, size;
 	char buff[250];
 	int bufflen;

 	assert(server != NULL);

 	server->nfds = 1;

	server->fds[0].fd = server->sock;
	server->fds[0].events = POLLIN;

 	while (server->running) {

 		printf("Waiting events...\n");
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

 							/*fcntl(newfd, F_SETFL, O_NONBLOCK);

 							server->fds[server->nfds].fd = newfd;
 							server->fds[server->nfds].events = POLLIN;
 							server->nfds++;*/
 						}

 					} while (newfd != -1);

 				}
 			}
 		}

 	}


 	close(server->sock);
 }

 void server_start(TServer *server) {

 	assert(server != NULL);

 	pthread_create(&server->thread, NULL, _server_thread_run, (void*) server);

 }