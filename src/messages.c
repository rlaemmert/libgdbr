#include "messages.h"
#include "arch.h"
#include "core.h"
#include "utils.h"


int handle_g(libgdbr_t* instance) {
	int index = instance->message_stack.count - 1;
	if (index < 0 ) {
		// theres no message that we could handle..
		return -1;
	}
	libgdbr_message_t* msg = &instance->message_stack.message_stack[index];
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
	unpack_hex(msg->msg, msg->len, instance->data);
	hexdump(instance->data, instance->data_len, 0);
	instance->message_stack.count--;
	free(msg->msg);
	return 0;
}


int handle_m(libgdbr_t* instance) {
	int index = instance->message_stack.count - 1;
	if (index < 0 ) {
		// theres no message that we could handle..
		return -1;
	}
	libgdbr_message_t* msg = &instance->message_stack.message_stack[index];
	//printf("Msg: %s with len: %i\n", msg->msg, msg->len); //TODO add debug flag here?
	instance->data_len = (msg->len / 2);
	unpack_hex(msg->msg, msg->len, instance->data);
	hexdump(instance->data, instance->data_len, 0);
	instance->message_stack.count--; 
	free(msg->msg);
	return 0;
}


int handle_cmd(libgdbr_t* instance) {
	printf("The last comand received: %i packets\n", instance->message_stack.count);
	libgdbr_message_t* current = &instance->message_stack.message_stack[0];
	while(current <= &instance->message_stack.message_stack[instance->message_stack.count	- 1]) {
		char* result = (char*) calloc((current->len / 2), sizeof(char));
		unpack_hex((current->msg), current->len, result);
		printf("Result: %s\n", result);
		hexdump(result, current->len / 2, 0);
		instance->message_stack.count--;
		current++;
	}
	return 0;
}


int handle_connect(libgdbr_t* instance) {
	int index = instance->message_stack.count - 1;
	if (index < 0) {
		printf("No answer!\n");
		return -1;
	}
	printf("Connect Parameter:\n");
	libgdbr_message_t* current = &instance->message_stack.message_stack[0];
	while(current < &instance->message_stack.message_stack[instance->message_stack.count	- 1]) {
		printf("Msg: %s\n", current->msg);
		instance->message_stack.count--;
		free(current->msg);
	}
	return 0;
}

