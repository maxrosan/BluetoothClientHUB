
#include "btlib.h"

static sdp_session_t* _describe_service(uint8_t port) {

	uint32_t service_uuid_int[] = {0x46077e11, 0xfde347d0, 0x8ece7dfc, 0x3ad411e4};
	uint8_t  rfcomm_channel = port;

	const char *service_name = "Mouse pointer bluetooth";
	const char *service_dsc = "Mouse pointer BT";
	const char *service_prov = "Roto-Rooter";

	uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid;

	sdp_list_t *l2cap_list = 0, 
    	*rfcomm_list = 0,
		*root_list = 0,
		*proto_list = 0, 
		*access_proto_list = 0;
		sdp_data_t *channel = 0, *psm = 0;

	int err = 0;
	sdp_session_t *session = 0;		

	sdp_record_t *record = sdp_record_alloc();

	// set the general service ID
	sdp_uuid128_create( &svc_uuid, &service_uuid_int );
	sdp_set_service_id( record, svc_uuid );

	// make the service record publicly browsable
	sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
	root_list = sdp_list_append(0, &root_uuid);
	sdp_set_browse_groups( record, root_list );	

	// set l2cap information
	sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
	l2cap_list = sdp_list_append( 0, &l2cap_uuid );
	proto_list = sdp_list_append( 0, l2cap_list );	

    // set rfcomm information
	sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
	channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
	rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
	sdp_list_append( rfcomm_list, channel );
	sdp_list_append( proto_list, rfcomm_list );	

	// attach protocol information to service record
	access_proto_list = sdp_list_append( 0, proto_list );
	sdp_set_access_protos( record, access_proto_list );

	// set the name, provider, and description
	sdp_set_info_attr(record, service_name, service_prov, service_dsc);	

    // connect to the local SDP server, register the service record, and 
    // disconnect
	session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );

	if (session == NULL) {
		fprintf(stderr, "sdp_connect failed\n");
		goto error;
	}

	err = sdp_record_register(session, record, 0);

error:
	// cleanup
	sdp_data_free( channel );
	sdp_list_free( l2cap_list, 0 );
	sdp_list_free( rfcomm_list, 0 );
	sdp_list_free( root_list, 0 );
	sdp_list_free( access_proto_list, 0 );

	return session;	
}

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

void _close_bt_sdp(sdp_session_t *session) {
	sdp_close(session);
}

int bt_server_init(Listener *l, int port) {

	assert(l != NULL && port >= 0);

	struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	socklen_t len = sizeof rem_addr;
	int fd, lm;
	sdp_session_t *session;

	if ((fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) {
		fprintf(stderr, "Failed to create socket\n");
		return FALSE;
	}

	lm = RFCOMM_LM_MASTER;

	if (setsockopt(fd, SOL_RFCOMM, RFCOMM_LM, &lm, sizeof(lm)) < 0) {
		fprintf(stderr, "Failed to set LM\n");
		close(fd);
		return FALSE;
	}

	loc_addr.rc_family  = AF_BLUETOOTH;
	loc_addr.rc_bdaddr  = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t) port;

	lm = RFCOMM_LM_MASTER;

	if (bind(fd, (struct sockaddr*) &loc_addr, len) < 0) {
		perror("bind");
		close(fd);
		return FALSE;
	}

	listen(fd, MAX_BT_CLIENTS);

	listener_add(l, fd);

	/*session = _describe_service(port);

	if (session != NULL) {
		fprintf(stderr, "Service registed\n");
		listener_register_close_callback(l, fd, _close_bt_sdp, session);
	} else {
		fprintf(stderr, "Failed to register service\n");
	}*/

	return TRUE;
}