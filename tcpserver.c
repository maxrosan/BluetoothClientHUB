
#include "tcpserver.h"

 int tcpserver_init(Listener *l, int port) {

 	assert(l != NULL);
 	assert(port > 0);

	struct sockaddr_in servaddr;
	int on = 1, fd;

 	fd = socket(AF_INET, SOCK_STREAM, 0);

 	//ioctl(fd, FIONBIO, (char*) &on, sizeof(on));

	memset((void*) &servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if (bind(fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
		perror("server_init");
		close(fd);

		return FALSE;
	}

	if (listen(fd, 1024) == -1) {
		perror("server_init");
		close(fd);

		return FALSE;
	}

	listener_add(l, fd);

	return TRUE;
 }

 