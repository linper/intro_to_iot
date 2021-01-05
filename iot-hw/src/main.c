
#include "iot-hw.h"

volatile int interrupt = 0;

enum {
	INFO_CODE,
	__INFO_MAX
};

enum {
	FREE_MEM,
	TOTAL_MEM,
	__MEM_MAX
};

static const struct blobmsg_policy dump_policy[__INFO_MAX] = {
	[INFO_CODE] = { .name = "memory", .type = BLOBMSG_TYPE_TABLE },
};

static const struct blobmsg_policy mem_policy[__MEM_MAX] = {
	[TOTAL_MEM] = { .name = "total", .type = BLOBMSG_TYPE_INT64 },
	[FREE_MEM] = { .name = "free", .type = BLOBMSG_TYPE_INT64 },
};

// Handle signal interrupt
void sigHandler(int signo) {
	printf("SigINT received.\n");
	interrupt = 1;
}

int get_ptr(struct uci_context* c, struct uci_ptr* ptr, char* path){
    int err;
	char pth[100];
	strcpy(pth, path);
    if((err = uci_lookup_ptr(c, ptr, pth, true)) != UCI_OK){
        printf("failed getting connfig entry");
		return -1;
    }
	return 0;
}

int get_config_data(char* org, char* type, char* id, char* token){
	struct uci_context *c;
	struct uci_ptr ptr;
	c = uci_alloc_context ();
	int err;
	
    err = get_ptr(c, &ptr, "iot-hw.main_sct.org");
	if(ptr.o != NULL && err == 0){
		strcpy(org, ptr.o->v.string);
	}else{
		uci_free_context(c);
		return -1;
	}

    err = get_ptr(c, &ptr, "iot-hw.main_sct.type");
	if(ptr.o != NULL && err == 0){
		strcpy(type, ptr.o->v.string);
	}else{
		uci_free_context(c);
		return -1;
	}

    err = get_ptr(c, &ptr, "iot-hw.main_sct.id");
	if(ptr.o != NULL && err == 0){
		strcpy(id, ptr.o->v.string);
	}else{
		uci_free_context(c);
		return -1;
	}

    err = get_ptr(c, &ptr, "iot-hw.main_sct.token");
	if(ptr.o != NULL && err == 0){
		strcpy(token, ptr.o->v.string);
	}else{
		uci_free_context(c);
		return -1;
	}
	
	uci_free_context(c);
	return 0;
}

int connect_device(iotfclient* client){
	char org[100];
	char type[100];
	char id[100];
	char token[100];
	int rc = -1;
	if(get_config_data(org, type, id, token) != 0){ //getting connention data
		printf("Getting connection config data failed.\n");
		return -1;
	}
	rc = initialize( //initializing client
		client,
		org,
		"internetofthings.ibmcloud.com",
		type,
		id,
		"token",
		token,
		"/etc/watson/cert/IoTFoundation.pem",
		0,
		NULL,
		NULL,
		NULL,
		0);
		
	if(rc != SUCCESS){
		printf("initialize failed with code: %d.\n", rc);
		return -1;
	}

	rc = connectiotf(client); //connenction client to cloud

	if(rc != SUCCESS){
		printf("Connection failed and returned rc = %d.\n\n", rc);
		syslog(LOG_ERR, "Connection to IBM cloud failed, with code %d for %s as %s at %s", rc, id, type, org);
		return -1;
	}else{
		syslog(LOG_NOTICE, "Connected to IBM cloud succeded for %s as %s at %s", id, type, org);
	}
	return 0;
}

int broadcast_data(struct ubus_context* ctx, iotfclient* client){
	float load;
	int rc = -1;
	char event[80];
	size_t event_len = sizeof(event);

	while(!interrupt){
		if(get_ram_data(ctx, &load) != 0){
			syslog(LOG_ERR, "Failed to receive system info from ubus server");
			return -1;
		}
		memset(event, '\0', event_len);
		sprintf(event, "{\"d\" : {\"data\" : %f }}", load);
		rc = publishEvent(client, "status","json", event, QOS0);
		printf(" %d, data: %f\n", rc, load);
		yield(client,1000);
		sleep(2);
	}
	return 0;
}

static void data_cb(struct ubus_request *req, int type, struct blob_attr *msg){
	struct blob_attr *tb[__INFO_MAX];
	struct blob_attr *mb[__MEM_MAX];
	float *load = (float*)req->priv;

	blobmsg_parse(dump_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[INFO_CODE]) {
		fprintf(stderr, "No return code received from server\n");
		return;
	}
	blobmsg_parse(mem_policy, __MEM_MAX, mb, blobmsg_data(tb[INFO_CODE]), blobmsg_data_len(tb[INFO_CODE]));
	if (!mb[TOTAL_MEM] || !mb[FREE_MEM]){
		fprintf(stderr, "not found\n");
		return;
	}else{
		float all = (float)blobmsg_get_u64(mb[TOTAL_MEM]);
		float free = (float)blobmsg_get_u64(mb[FREE_MEM]);
		*load = (1 - (free / all)) * 100;
	}
}

int get_ram_data(struct ubus_context* ctx, float* load){
	static struct blob_buf b;
	*load = -1.0;
	uint32_t uid;
	if(ubus_lookup_id(ctx, "system", &uid)){
		printf("Failed to lookup.\n");
		return -1;
	}
	blob_buf_init(&b, 0);
	if(ubus_invoke(ctx, uid, "info", b.head, data_cb, load, 5000) != 0){
		printf("Failed to call.\n");
		return -1;
	}
	if(*load < 0){
		printf("Failed to get data from ubus.\n");
		return -1;
	}
	return 0;
}

int main(int argc, char const *argv[]){
	iotfclient client;
	struct ubus_context* ctx;

	openlog (NULL, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	//catch interrupt signal
	signal(SIGINT, sigHandler);
	signal(SIGTERM, sigHandler);
		
	if(!(ctx = ubus_connect(NULL))){ //TODO:should log error
		syslog(LOG_ERR, "Failure while connectiong to ubus");
		goto exit;
	}

	if(connect_device(&client) != 0){
		goto exit_free_ubus;
	}
	
	broadcast_data(ctx, &client);

	//================================
	//exiting
	//================================
	exit_disconnenct_client:
		printf("Dicsonnencting...\n");
		disconnect(&client);
	exit_free_ubus:
		ubus_free(ctx);
	exit:
		printf("Quitting...\n");
		closelog ();
		return 0;
}
