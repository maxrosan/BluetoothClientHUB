
#include <assert.h>
#include "msg.h"

static const char msg_magic_number[] = {'b', 't', 'h', 'b'};

Message* msg_create(int proto_from, int from, int proto_to, int to, char *msg) {

	Message *res;

	res = (Message*) malloc(sizeof(Message));

	res->from_proto = proto_from;
	res->from_fd    = from;

	res->to_fd    = to;
	res->to_proto = proto_to;

	res->msg     = strdup(msg);
	res->size    = strlen(msg);

	return res;

}

int msg_size_serialized(Message *msg) {

	assert(msg != NULL);

	int pt = 0;

	//memcpy(pt, msg_magic_number, sizeof msg_magic_number);
	pt = pt + (sizeof msg_magic_number);

	//memcpy(pt, (char*) &msg->from_proto, sizeof(msg->from_proto));
	pt = pt + sizeof(msg->from_proto);
	
	//memcpy(pt, (char*) &msg->from_fd, sizeof(msg->from_fd));
	pt = pt + sizeof(msg->from_fd);

	//memcpy(pt, (char*) &msg->to_proto, sizeof(msg->to_proto));
	pt = pt + sizeof(msg->to_proto);	

	//memcpy(pt, (char*) &msg->to_fd, sizeof(msg->to_fd));
	pt = pt + sizeof(msg->to_fd);

	//memcpy(pt, (char*) &msg->size, sizeof(msg->size));
	pt = pt + sizeof(msg->size);

	//memcpy(pt, msg->msg, msg->size);
	pt = pt + msg->size;

	return pt;
}

void msg_serialize(Message *msg, char** res, int *len) {
	char *pt;

	assert(msg != NULL && res != NULL && *res != NULL && len != NULL);

	pt = *res;

	memcpy(pt, msg_magic_number, sizeof msg_magic_number);
	pt = pt + (sizeof msg_magic_number);

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

	*len = (pt - *res);
}

Message* msg_deserialized(char *buff) {

	char *pt;
	char msg_magic_number_tmp[sizeof msg_magic_number];
	Message *msg = NULL;

	assert(buff);

	pt = buff;

	memcpy(msg_magic_number_tmp, pt, sizeof msg_magic_number);
	pt = pt + (sizeof msg_magic_number);

	if (memcmp(msg_magic_number_tmp, msg_magic_number, sizeof msg_magic_number) != 0) {
		fprintf(stderr, "Invalid magic number\n");
		return NULL;
	}

	msg = (Message *) malloc(sizeof(Message));

	memcpy((char*) &msg->from_proto, pt, sizeof(msg->from_proto));
	pt = pt + sizeof(msg->from_proto);
	
	memcpy((char*) &msg->from_fd, pt, sizeof(msg->from_fd));
	pt = pt + sizeof(msg->from_fd);

	memcpy((char*) &msg->to_proto, pt, sizeof(msg->to_proto));
	pt = pt + sizeof(msg->to_proto);	

	memcpy((char*) &msg->to_fd, pt, sizeof(msg->to_fd));
	pt = pt + sizeof(msg->to_fd);

	memcpy((char*) &msg->size, pt, sizeof(msg->size));
	pt = pt + sizeof(msg->size);

	memcpy(msg->msg, pt, msg->size);

	return msg;
}

void msg_free(Message *msg) {
	if (msg) {
		free(msg);
	}
}

void msg_destroy(Message *msg) {
	free(msg->msg);
	free(msg);
}