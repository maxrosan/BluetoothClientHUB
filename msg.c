

Message* msg_create(int from, int to, char *msg) {

	Message *res;

	res = (Message*) malloc(sizeof(Message));

	res->from_fd = from;
	res->to_fd   = to;
	res->msg     = strdup(msg);

	return res;

}

void msg_serialize(Message *msg, char** res, int *len) {
	char *pt;

	pt = *res;
	memcpy(pt, (char*) &msg->from_fd, sizeof(msg->from_fd));
	memcpy(pt + sizeof(msg->to_fd), (char*) &msg->to_fd, sizeof(msg->to_fd));
	memcpy(pt + sizeof(msg->to_fd) + sizeof(msg->from_fd),
		msg->msg, msg->size);
}

void msg_destroy(Message *msg) {
	free(msg->msg);
	free(msg);
}