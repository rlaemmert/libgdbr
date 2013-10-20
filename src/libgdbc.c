#include "libgdbc.h"
#include "core.h"

#include <stdio.h>


libgdbc_t instance;


int libgdbc_init() {
	return create_instance(&instance);
}


int libgdbc_cleanup() {
	return delete_instance(&instance);
}


int libgdbc_connect(char* server, int port) {
	return connect_instance(&instance, server, port);
}


int libgdbc_disconnect() {
	return disconnect_instance(&instance);
}


int libgdbc_continue() {
	return continue_instance(&instance);
}


int libgdbc_read_registers() {
	return regread_instance(&instance);
}

int libgdbc_send_cmd(char* command) {
	int acks = instance.acks;
	if (send_command(&instance, command) == 0) {
		if (acks < instance.acks) {
			/* the last send_command was a success so handling the answer now */
			char* answer = pop_message(&instance);
			instance.acks--;
			printf("Msg: %s\n", answer);
			free(answer);
		}
	} else {
		return -1;
	}
}
