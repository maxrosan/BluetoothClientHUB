
#include <assert.h>
#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>

#include "msg.h"
#include "util.h"

Message* msg_create(int from, int to, char *msg) {

	Message *res;

	res = (Message*) malloc(sizeof(Message));

	res->type = MESSAGE;
	res->from = from;
	res->to   = to;
	res->msg  = strdup(msg);

	return res;

}

Message* msg_create_without_msg(int from, int to) {

	Message *res;

	res = (Message*) malloc(sizeof(Message));

	res->type = MESSAGE;
	res->from = from;
	res->to   = to;
	res->msg  = NULL;

	return res;

}

Message* msg_copy(Message *msg) {

	Message *res;

	res = (Message*) malloc(sizeof(Message));

	res->type = msg->type;
	res->from = msg->from;
	res->to   = msg->to;
	res->msg  = strdup(msg->msg);

	return res;
}

Message* msg_from_json(int from, const char *msgtxt) {

	Message *ret;
	int to;
	const char *txt;
	JsonParser *parser;
	JsonReader *reader;
	char *cpy;

	parser = json_parser_new();
	json_parser_load_from_data(parser, msgtxt, -1, NULL);

	reader = json_reader_new(json_parser_get_root(parser));

	json_reader_read_member(reader, "to");
	to = json_reader_get_int_value(reader);
	json_reader_end_member(reader);

	json_reader_read_member(reader, "txt");
	txt = json_reader_get_string_value(reader);
	json_reader_end_member(reader);	

	cpy = strdup(txt);

	g_object_unref(reader);
	g_object_unref(parser);

	ret = msg_create_without_msg(from, to);
	ret->msg = cpy;

	return ret;
}

char *msg_to_json(Message *msg) {

	JsonBuilder *builder;
	JsonGenerator *gen;
	JsonNode *node;
	char *ret;

	builder = json_builder_new();

	json_builder_begin_object(builder);

	json_builder_set_member_name(builder, "to");
	json_builder_add_int_value(builder, msg->to);

	json_builder_set_member_name(builder, "from");
	json_builder_add_int_value(builder, msg->from);

	json_builder_set_member_name(builder, "txt");
	json_builder_add_string_value(builder, msg->msg);

	json_builder_end_object(builder);

	gen = json_generator_new();
	node = json_builder_get_root(builder);
	json_generator_set_root(gen, node);

	ret = (char*) json_generator_to_data(gen, NULL);

	DEBUG("msg [%d %d %s]", msg->from, msg->to, msg->msg);
	DEBUG("ret = %s", ret);

	json_node_free(node);
	g_object_unref(gen);
	g_object_unref(builder);

	return ret;
}

Message* msg_get_quit_msg() {
	static Message* msg = NULL;

	if (msg == NULL) {
		msg = msg_create(-1, -1, "QUIT");
		msg->type = SIG_QUIT;
	}

	return msg;
}

void msg_destroy(Message *msg) {
	free(msg->msg);
	free(msg);
}