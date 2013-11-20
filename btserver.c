
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "tcpserver.h"
#include "util.h"
#include "listener.h"

static Listener listener;

static void _sig_handler(int sig) {

	DEBUG("Signal %d caught", sig);

	if (sig == SIGINT) {
		listener_close(&listener);
	}
}

int main(int argc, char **argv) {

	signal(SIGPIPE, _sig_handler);
	signal(SIGINT, _sig_handler);

	listener_init(&listener);
	tcpserver_init(&listener, atoi(argv[1]));
	listener_run(&listener);

	//bt_test();

	/*server_init(&server, atoi(argv[1]));
	server_start(&server);

	fprintf(stderr, "Starting sender\n");
	while (TRUE) {
		sleep(3);

		fprintf(stderr, "Sending msg\n");
		server_send_msg(&server, "HELLO");
	}*/

	return EXIT_SUCCESS;
}