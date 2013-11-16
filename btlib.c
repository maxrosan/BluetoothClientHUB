
#include "btlib.h"

int bt_search(const char *btname) {

	inquiry_info *ii;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	int res = FALSE;

	char addr[19] = {0};
	char name[248] = {0};

	dev_id = hci_get_route(NULL);
	sock   = hci_open_dev(dev_id);

	if (dev_id < 0 || sock < 0) {
		perror("bt_search");
		return FALSE;
	}

	fprintf(stderr, "Device %d found\n", dev_id);

	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;

	ii = (inquiry_info*) malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);

	if (num_rsp < 0) {
		perror("hci_inquiry");
		return FALSE;
	}



	for (i = 0; i < num_rsp; i++) {
		ba2str(&(ii + i)->bdaddr, addr);
		memset(name, 0, sizeof name);

		if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof name, name, 0) < 0) {
			strcpy(name, "[unknown]");
		}


		fprintf(stderr, "%s %s\n", addr, name);
	}

	return res;
}

int bt_server_init(BTServer *server, int port) {

	assert(server != NULL && port >= 0);

	struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	socklen_t len = sizeof rem_addr;

	if ((server->sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) {
		return FALSE;
	}

	loc_addr.rc_family  = AF_BLUETOOTH;
	loc_addr.rc_bdaddr  = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t) port;

	if (bind(server->sock, (struct sockaddr*) &loc_addr, len) < 0) {
		perror("bind");
		close(server->sock);
		return FALSE;
	}

	listen(server->sock, MAX_BT_CLIENTS);

	return TRUE;
}

void bt_server_run(BTServer *server, void (*callback)(int from, char *msg, int len)) {

	assert(server != NULL);

	int i, size, client, bytes_read, j;
	char buf[256];

 	server->nfds = 1;
	server->fds[0].fd = server->sock;
	server->fds[0].events = POLLIN;

	for (j = 1; j <= MAX_BT_CLIENTS; j++) {
		server->fds[j].fd = -1;
	}

	server->running = TRUE;

 	while (server->running) {

 		fprintf(stderr, "BT server: Waiting events...\n");

 		poll(server->fds, server->nfds, 3 * 1000);

 		size = server->nfds;
 		for (i = 0; i < size; i++) {
 			if (server->fds[i].revents != 0) {
 				if (server->fds[i].revents != POLLIN) {
 					server->running = FALSE;
 				} else if (server->fds[i].fd == server->sock) {
 					client = accept(server->sock, NULL, NULL);

 					for (j = 1; j <= MAX_BT_CLIENTS; j++) {
 						if (server->fds[j].fd == -1) {
 							server->fds[j].fd = client;
 							server->fds[j].events = POLLIN;
 							server->nfds++;
 							j = MAX_BT_CLIENTS + 1;

 							fprintf(stderr, "BT %d connected\n", client);
 						}
 					}
 				} else {
 					client = server->fds[i].fd;
 					do {
 						bytes_read = read(client, buf, sizeof buf);
 					} while (bytes_read == -1 && errno == EAGAIN);

 					if (bytes_read == -1) {
 						close(server->fds[i].fd);
 						server->fds[i].fd = -1;
 						server->nfds--;
 					} else {
 						fprintf(stderr, "Msg from %d\n", client);

 						callback(server->fds[i].fd, buf, bytes_read);
 					}
 				}
 			}
 		}
 	}

}

void _bt_callback(int from, char *msg, int len) {
	fprintf(stderr, "BT msg from %d\n", from);
}

void bt_test(void) {

	BTServer server;
	if (bt_server_init(&server, 1)) {
		bt_server_run(&server, _bt_callback);
	} else {
		fprintf(stderr, "Failed to init BT\n");
	}
}