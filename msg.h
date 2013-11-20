
#ifndef MSG_H
#define MSG_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

typedef struct _Message {

	enum { MESSAGE, SIG_QUIT } type;

	int from;
	int to;

	char *msg;

} Message;

Message* msg_create(int from, int to, char *msg);
Message* msg_copy(Message *msg);
Message* msg_create_without_msg(int from, int to);
char *msg_to_json(Message *msg);
Message* msg_from_json(int from, const char *msg);
Message* msg_get_quit_msg();
void msg_destroy(Message *msg);

#endif