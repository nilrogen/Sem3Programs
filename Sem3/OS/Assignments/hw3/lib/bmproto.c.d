#include <bmproto.h>

#include <stdio.h>

extern bmproto_t bmp_setup(int optype) {
	bmproto_t ret;
	ret.type = BMP_SETUP;
	ret.message.setup_msg.operation_type = optype;
	return ret;
}

extern bmproto_t bmp_reply(int status, int selection, int value) {
	bmproto_t ret;
	ret.type = BMP_REPLY;
	ret.message.reply_msg.status         = status;
	ret.message.reply_msg.selection      = selection;
	ret.message.reply_msg.value          = value;
	return ret;
}

extern bmproto_t bmp_operation(int optype, int selection) {
	bmproto_t ret;
	ret.type = BMP_OPERATION;
	ret.message.operation_msg.operation_type = optype;
	ret.message.operation_msg.selection      = selection;
	return ret;
}

extern bmproto_t bmp_produce(int selection) {
	return bmp_operation(BMP_PRODUCE, selection);
}

extern bmproto_t bmp_consume(int selection) {
	return bmp_operation(BMP_CONSUME, selection);
}

extern bmproto_t bmp_destroy() {
	bmproto_t ret;
	//memset(&ret, 0, sizeof(bmproto_t))
	ret.type = BMP_DESTROY;
	return ret;
}

/*
extern int bmp_opnum(bmproto_t proto)  {
	switch (proto.type) {
	case BMP_REPLY:     return proto.message.reply_msg.operation_num;
	case BMP_OPERATION: return proto.message.operation_msg.operation_num;
	case BMP_DESTROY:   return proto.message.destroy_msg.operation_num;
	}
	return -1;
}
*/
extern int bmp_optype(bmproto_t proto)  {
	switch (proto.type) {
	case BMP_SETUP:     return proto.message.setup_msg.operation_type;
	case BMP_OPERATION: return proto.message.operation_msg.operation_type;
	}
	return -1;
}
extern int bmp_selection(bmproto_t proto)  {
	switch (proto.type) {
	case BMP_REPLY:     return proto.message.reply_msg.selection;
	case BMP_OPERATION: return proto.message.operation_msg.selection;
	}
	return -1;
}
  
extern int bmp_value(bmproto_t proto)  {
	switch (proto.type) {
	case BMP_REPLY:     return proto.message.reply_msg.value;
	}
	return -1;
}

extern int bmp_status(bmproto_t proto) {
	if (proto.type == BMP_REPLY) {
		return proto.message.reply_msg.status;
	}
	return -1;
}

extern void bmp_print(const bmproto_t msg) {
	int optype = bmp_optype(msg);
	int selection = bmp_selection(msg);
	int value = bmp_selection(msg);

	printf("Buffer Manager Protocol Message Info:\n");
	printf("type: ");
	switch(msg.type) {
	case BMP_SETUP:
		printf("BMP_SETUP (%d)\n", BMP_SETUP);
		printf("operation_type: %d (%s)\n", optype, 
		       (optype == BMP_PRODUCE ? "BMP_PRODUCE" : "BMP_CONSUME"));
		break;
	case BMP_REPLY:
		printf("BMP_REPLY (%d)\n", BMP_REPLY);
		printf("operation_status: %d ", msg.message.reply_msg.status);
		printf("%s\n", (msg.message.reply_msg.status == BMP_SUCCESS ? 
						"BMP_SUCCESS" : "BMP_FAILURE"));
		printf("selection: %d\n",      selection);
		printf("value: %d\n",          value);
		break;
	case BMP_OPERATION:
		printf("BMP_OPERATION (%d)\n", BMP_OPERATION);
		printf("operation_type: %d (%s)\n", optype, 
		       (optype == BMP_PRODUCE ? "BMP_PRODUCE" : "BMP_CONSUME"));
		printf("selection: %d\n",      selection);
		break;
	case BMP_DESTROY:
		printf("BMP_DESTROY (%d)\n", BMP_DESTROY);
		break;
	}
	printf("\n");
}


