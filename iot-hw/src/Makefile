BIN:=iot-hw
LDFLAGS:=-liotfdeviceclient -liotfdevicemgmt -liotfgatewayclient -luci -lubus -lubox -lblobmsg_json
CFLAGS=-Wall

.PHONY: all clean

all: $(BIN)

$(BIN): main.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(BIN)
