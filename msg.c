
#include <assert.h>
#include "msg.h"

Message* msg_create(int proto_from, int from, int proto_to, int to, char *msg) {

	Message *res;

	res = (Message*) malloc(sizeof(Message));

	res->from_proto = proto_from;
	res->from_fd    = from;

	res->to_fd    = to;
	res->to_proto = proto_to;

	res->msg     = strdup(msg);
	res->size    = strlen(msg) + 1;

	return res;

}

void msg_serialize(Message *msg, char** res, int *len) {
	char *pt;

	assert(msg != NULL && res != NULL && *res != NULL && len != NULL);

	pt = *res;

	memcpy(pt, (char*) &msg->from_proto, sizeof(msg->from_proto));
	pt = pt + sizeof(msg->from_proto);
	
	memcpy(pt, (char*) &msg->from_fd, sizeof(msg->from_fd));
	pt = pt + sizeof(msg->from_fd);

	memcpy(pt, (char*) &msg->to_proto, sizeof(msg->to_proto));
	pt = pt + sizeof(msg->to_proto);	

	memcpy(pt, (char*) &msg->to_fd, sizeof(msg->to_fd));
	pt = pt + sizeof(msg->to_fd);

	memcpy(pt, (char*) &msg->size, sizeof(msg->size));
	pt = pt + sizeof(msg->size);

	memcpy(pt, msg->msg, msg->size);
	pt = pt + msg->size;

	*len = (pt - *res) + 1;

}

void msg_destroy(Message *msg) {
	free(msg->msg);
	free(msg);
}