
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "tcpserver.h"
#include "util.h"

static TServer server;

static void _sig_handler(int sig) {

	fprintf(stderr, "Signal %d caught\n", sig);

}

int main(int argc, char **argv) {

	signal(SIGPIPE, _sig_handler);

	server_init(&server, atoi(argv[1]));
	server_start(&server);

	fprintf(stderr, "Starting sender\n");
	while (TRUE) {
		sleep(3);

		fprintf(stderr, "Sending msg\n");
		server_send_msg(&server, "HELLO");
	}

	return EXIT_SUCCESS;
}