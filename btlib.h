
#include "util.h"

#include <sys/poll.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <assert.h>

#define MAX_BT_CLIENTS 1024

typedef struct _BTServer {

	int sock;
	int clients[MAX_BT_CLIENTS];
	int n_clients;

	struct pollfd fds[MAX_BT_CLIENTS + 1];
	int nfds;
	int running;

} BTServer;

int bt_search(const char *name);
int bt_server_init(BTServer *server, int port);
void bt_server_run(BTServer *server, void (*callback)(int from, char *msg, int len));

void bt_test(void);