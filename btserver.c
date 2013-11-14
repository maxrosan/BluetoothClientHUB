
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tcpserver.h"
#include "util.h"

static TServer server;

int main(int argc, char **argv) {

	server_init(&server, atoi(argv[1]));
	server_start(&server);

	while (TRUE) {
		sleep(3);
	}

	return EXIT_SUCCESS;
}