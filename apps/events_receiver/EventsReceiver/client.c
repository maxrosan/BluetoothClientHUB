
#include "client.h"

#include <assert.h>

Client *client_init(const char *address, int port) {

	struct sockaddr_in serv_addr;
	Client *client = NULL;
	int fd;

	memset(&serv_addr, 0, sizeof(serv_addr));

	fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0) {
		fprintf(stderr, "Failed to open socket\n");
		return NULL;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, address, &serv_addr.sin_addr) != 0) {
		fprintf(stderr, "Failed to convert address\n");
		close(fd);
		return NULL;
	}

	if (connect(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "Failed to connect\n");
		close(fd);
		return NULL;
	}

	client = (Client*) malloc(sizeof(Client));

	client->sockfd = fd;
	client->running = TRUE;

	return client;
}

int _json_get_int_val(JsonReader *reader, const char *name) {
	int retval;

	json_reader_read_member(reader, name);
	retval = json_reader_get_int_value(reader);
	json_reader_end_member(reader);	

	return retval;
}

gdouble _json_get_double_val(JsonReader *reader, const char *name) {
	double retval;

	json_reader_read_member(reader, name);
	retval = json_reader_get_double_value(reader);
	json_reader_end_member(reader);	

	return retval;
}

inline static void _decode_internal_message(Message *msg, const char *txt) {

	char *ptr = NULL;

	ptr = strtok(txt, ";");
	msg->x = atof(ptr);

	ptr = strtok(NULL, ";");
	msg->y = atof(ptr);

	ptr = strtok(NULL, ";");
	msg->z = atof(ptr);

}

inline static Message* _decode_message(char *msgtxt) {

	int to, from;
	const char *txt;
	JsonParser *parser;
	JsonReader *reader;
	char *cpy;
	Message *msg;

	parser = json_parser_new();
	json_parser_load_from_data(parser, msgtxt, -1, NULL);

	reader = json_reader_new(json_parser_get_root(parser));

	to = _json_get_int_val(reader, "to");

	from = _json_get_int_val(reader, "from");

	json_reader_read_member(reader, "txt");
	txt = json_reader_get_string_value(reader);
	json_reader_end_member(reader);	

	msg = (Message*) malloc(sizeof(Message));

	msg->from = from;
	msg->to = to;

	_decode_internal_message(msg, txt);

	g_object_unref(reader);
	g_object_unref(parser);

	return msg;

}

void _free_msg(Message *msg) {
	free(msg);
}

void client_main_loop(Client *cl, void (*callback)(Message *msg)) {

	assert(cl != NULL);

	int bytes_read;
	struct pollfd pr;
	Message *msg;

	pr.fd = cl->sockfd;
	pr.events = POLLIN;	

	while (cl->running) {

		poll(&pr, 1, 3 * 1000);

		if (pr.revents == POLLIN) {

			do { 
				bytes_read = read(cl->sockfd, cl->buffer, sizeof cl->buffer);
			} while (bytes_read == -1 && errno == EAGAIN);	

			if (bytes_read > 0) {

				fprintf(stderr, "Msg: %s\n", cl->buffer);

				msg = _decode_message(cl->buffer);

				callback(msg);

				_free_msg(msg);
			}

		} else if (pr.revents != 0) {
			cl->running = FALSE;

			fprintf(stderr, "Exitting program");
		}
	}

}
