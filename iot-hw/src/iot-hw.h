#ifndef IOT_HW_H
#define IOT_HW_H

#include <deviceclient.h>
#include <uci.h>
#include <libubox/blobmsg_json.h>
#include <libubox/blobmsg.h>
#include "libubus.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

void sigHandler(int signo);

int get_ptr(struct uci_context* c, struct uci_ptr* ptr, char* path);

int get_config_data(char* org, char* type, char* id, char* token);

int connect_device(iotfclient* client);

int broadcast_data(struct ubus_context* ctx, iotfclient* client);

static void data_cb(struct ubus_request *req, int type, struct blob_attr *msg);

int get_ram_data(struct ubus_context* ctx, float* load);

#endif