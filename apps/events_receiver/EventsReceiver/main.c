#include <stdio.h>
#include <stdlib.h>

#include "events_sender.h"
#include "client.h"

static EventsSender *es;
static Client *client;

static void callback(Message *msg) {

	 events_sender_mouse_move(es, (int) msg->x, (int) msg->y);

}

int main(int argc, char** argv) {

	//__events_sender_tester();

	char *hostname = argv[1];
	int port = atoi(argv[2]);

	es = events_sender_create();
	client = client_init(hostname, port);

	client_main_loop(client, callback);

	return (EXIT_SUCCESS);
}

