
#include "events_sender.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <unistd.h>

void _getCenterOfScreen(Display *display, int *x, int *y) {

	Screen *screen = DefaultScreenOfDisplay(display);

	assert(screen != NULL && x != NULL && y != NULL);

	*x = screen->width  >> 1;
	*y = screen->height >> 1;
}


void _centerPointer(EventsSender *es) {

	_getCenterOfScreen(es->display, &es->positionMouseX, &es->positionMouseY);

	fprintf(stderr, "Centering pointer %d %d\n", es->positionMouseX, es->positionMouseY);

	events_sender_mouse_move(es, 0, 0);
}

EventsSender *events_sender_create(void) {

	EventsSender *es = (EventsSender*) malloc(sizeof(EventsSender));

	es->display = XOpenDisplay(NULL);

	assert(es->display != NULL);

	_centerPointer(es);

	return es;
}

void events_sender_mouse_move(EventsSender *es, int x, int y) {

	assert(es != NULL);

	x += es->positionMouseX;
	y += es->positionMouseY;

	int result = XTestFakeMotionEvent(es->display, 0, x, y, 0);
	XFlush(es->display);

	fprintf(stderr, "result = %d\n", result);

	es->positionMouseX = x;
	es->positionMouseY = y;
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
