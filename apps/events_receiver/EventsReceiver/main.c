#include <stdio.h>
#include <stdlib.h>

#include "events_sender.h"
#include "client.h"

#define XK_Up                            0xff52  /* Move up, up arrow */
#define XK_Down                          0xff54  /* Move down, down arrow */

static EventsSender *es;
static Client *client;

#define NOISE 0.2

float velocity[2][2] = {{0, 0}, {0, 0}};

#define X 0
#define Y 1

#define INTERVAL 0.1

static void callback(Message *msg) {

	/*float dx, dy;

	msg->x *= 100;
	msg->y *= 10;

	velocity[X][1] = velocity[X][0] + msg->x * INTERVAL;
	velocity[Y][1] = velocity[Y][0] + msg->y * INTERVAL;

	dx = velocity[X][1] * INTERVAL;
	dy = velocity[Y][1] * INTERVAL;

	events_sender_mouse_move(es, (int) dx, (int) dy);

	velocity[X][0] = velocity[X][1];
	velocity[Y][0] = velocity[Y][1];*/

	fprintf(stderr, "msg->type = %d\n", msg->type);

	switch (msg->type) {
		case VOLUME_UP:
			event_sender_keyboard_event(es, XK_Up);
			break;
		case VOLUME_DOWN:
			event_sender_keyboard_event(es, XK_Down);
			break;			
	}

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

