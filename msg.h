
#ifndef MSG_H
#define MSG_H

#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

enum ProtocolEnum {
	PROTO_ALL,
	PROTO_BT,
	PROTO_IP,
	__MAX_PROTO_NUM
};

typedef struct _Message {

	int from_proto;
	int from_fd;

	int to_proto;
	int to_fd;

	char *msg;
	int  size;

} Message;

Message* msg_create(int proto_from, int from, int proto_to, int to, char *msg);
void msg_serialize(Message *msg, char** res, int *len);
void msg_destroy(Message *msg);

#endif