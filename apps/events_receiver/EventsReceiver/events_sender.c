
#include "events_sender.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <unistd.h>

EventsSender *events_sender_create(void) {

	EventsSender *es = (EventsSender*) malloc(sizeof(EventsSender));

	es->display = XOpenDisplay(NULL);

	printf("display 0x%x", es->display);

	return es;
}

void events_sender_mouse_move(EventsSender *es, int x, int y) {

	assert(es != NULL);

	XTestFakeMotionEvent(es->display, 0, x, y, 0);
	XFlush(es->display);
}


void __events_sender_tester(void) {

	EventsSender *es = events_sender_create();
	int i;

	for (i = 0; i < 100; i+= 10) {
		events_sender_mouse_move(es, 100 + i, 500);
		printf("i = %d\n", i);
		sleep(1);
	}	
}
