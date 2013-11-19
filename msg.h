
#ifndef MSG_H
#define MSG_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

typedef struct _Message {

	enum type { MESSAGE, SIG_QUIT };

	int from_fd;
	int to_fd;

	char *msg;
	int  size;

} Message;

Message* msg_create(int proto_from, int from, int proto_to, int to, char *msg);
void msg_serialize(Message *msg, char** res, int *len);
Message* msg_deserialized(char *buff);
void msg_free(Message *msg);
void msg_destroy(Message *msg);

#endif