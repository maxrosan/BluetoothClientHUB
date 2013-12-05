
#include "client.h"

#include <assert.h>

Client *client_init(const char *address, int port) {

	struct sockaddr_in serv_addr;
	Client *client = NULL;
	int fd;

	memset(&serv_addr, 0, sizeof(serv_addr));

	fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0) {
		fprintf(stderr, "Failed to open socket\n");
		return NULL;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, address, &serv_addr.sin_addr) != 0) {
		fprintf(stderr, "Failed to convert address\n");
		close(fd);
		return NULL;
	}

	if (connect(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "Failed to connect\n");
		close(fd);
		return NULL;
	}

	client = (Client*) malloc(sizeof(Client));
	client->sockfd = fd;

	return client;
}

void client_main_loop(Client *client) {

	assert(client != NULL);

}
