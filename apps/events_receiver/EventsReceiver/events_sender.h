
#ifndef EVENTS_SENDER_H
#define	EVENTS_SENDER_H

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

typedef struct _EventsSender {
    Display *display;
} EventsSender;

EventsSender *events_sender_create(void);
void events_sender_mouse_move(EventsSender*, int x, int y);
void __events_sender_tester(void);
#endif	/* EVENTS_SENDER_H */

