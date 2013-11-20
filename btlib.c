
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

int bt_server_init(Listener *l, int port) {

	assert(l != NULL && port >= 0);

	struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	socklen_t len = sizeof rem_addr;
	int fd;

	if ((fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) {
		return FALSE;
	}

	loc_addr.rc_family  = AF_BLUETOOTH;
	loc_addr.rc_bdaddr  = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t) port;

	if (bind(fd, (struct sockaddr*) &loc_addr, len) < 0) {
		perror("bind");
		close(fd);
		return FALSE;
	}

	listen(fd, MAX_BT_CLIENTS);

	listener_add(l, fd);

	return TRUE;
}