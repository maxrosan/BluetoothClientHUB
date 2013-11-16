
typedef struct _Message {

	int from_fd;
	int to_fd;

	char *msg;
	int  size;

} Message;

Message* msg_create(int from, int to, char *msg);
void msg_serialize(Message *msg, char** res, int *len);
void msg_destroy(Message *msg);