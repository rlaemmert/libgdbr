#include "messages.h"
#include "arch.h"
#include "core.h"
#include "utils.h"


int handle_g(libgdbc_t* instance) {
	int index = instance->message_stack.count - 1;
	if (index < 0 ) {
		// theres no message that we could handle..
		return -1;
	}
	libgdbc_message_t* msg = &instance->message_stack.message_stack[index];
	//printf("Msg: %s with len: %i\n", msg->msg, msg->len);
	
	int len = msg->len;
	int i = 0;
	printf("Len: %i\n", len);
	while(len > 0) {
		printf("reg[%i] %s = ", i, x86_64[i].name);
		printf("%016llx\n", unpack_uint64_co ((msg->msg + x86_64[i].offset * 2), x86_64[i].size * 2));
		len -= x86_64[i++].size * 2;
	}
	// printf("Msg: %s with len: %i\n", msg->msg, msg->len); //TODO Debug...
	//instance->data = calloc((msg->len / 2), sizeof(char));
	instance->data_len = (msg->len / 2);
	
	i = 0;
	while (i <= msg->len / 2) {
		int val = hex2int(msg->msg[(i*2)]);
		val <<= 4;
		val |= hex2int(msg->msg[(i*2)+1]);
		instance->data[i++] = val;
	}
	//hexdump(instance->data, instance->data_len, 0);
	return 0;
}


int handle_m(libgdbc_t* instance) {
	int index = instance->message_stack.count - 1;
	if (index < 0 ) {
		// theres no message that we could handle..
		return -1;
	}
	libgdbc_message_t* msg = &instance->message_stack.message_stack[index];
	//printf("Msg: %s with len: %i\n", msg->msg, msg->len); //TODO add debug flag here?
	instance->data_len = (msg->len / 2);
	
	int i = 0;
	while (i <= msg->len / 2) {
		int val = hex2int(msg->msg[(i*2)]);
		val <<= 4;
		val |= hex2int(msg->msg[(i*2)+1]);
		instance->data[i++] = val;
	}
	hexdump(instance->data, instance->data_len, 0);
	instance->message_stack.count--; 

	return 0;
}
