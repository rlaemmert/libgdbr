#include "messages.h"
#include "arch.h"

int handle_g(libgdbc_t* instance) {
	libgdbc_message_t* msg = &instance->message_stack.message_stack[--instance->message_stack.top];
	printf("Msg: %s\n", msg->msg);
	
	
	int len = msg->len;
	int i = 0;
	printf("Len: %i\n", len);
	while(len > 0) {
		printf("reg[%i] %s = ", i, x86_64[i].name);
		printf("%016llx\n", unpack_uint64_co ((msg->msg + x86_64[i].offset * 2), x86_64[i].size * 2));
		len -= x86_64[i++].size * 2;
	}
	return 0;
}
