#include "libgdbr.h"
#include "core.h"

#include <stdio.h>


libgdbr_t instance;


//int libgdbr_init(libgdbr_t* instance, uint8_t architecture) {
//	return create_instance(&instance, architecture);
//}
//
//
//int libgdbr_cleanup() {
//	return delete_instance(&instance);
//}
//
//
//int libgdbr_connect(char* server, int port) {
//	return connect_instance(&instance, server, port);
//}
//
//
//int libgdbr_disconnect() {
//	return disconnect_instance(&instance);
//}
//
//
//int libgdbr_continue() {
//	return continue_instance(&instance);
//}
//
//
//int libgdbr_read_registers() {
//	return regread_instance(&instance);
//}
//
//
//int libgdbr_read_memory(uint64_t addr, uint64_t len) {
//	return memread_instance(&instance, addr, len);
//}


int libgdbr_send_cmd(char* command) {
	int acks = instance.acks;
	if (send_command(&instance, command) == 0) {
		if (acks < instance.acks) {
			/* the last send_command was a success so handling the answer now */
			char* answer = pop_message(&instance);
			instance.acks--;
//			printf("Msg: %s\n", answer);
			free(answer);
		}
	} else {
		return -1;
	}
}
