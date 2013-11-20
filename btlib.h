
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

#include "listener.h"

#define MAX_BT_CLIENTS 1024

int bt_search(const char *name);
int bt_server_init(Listener *l, int port);