#include "libgdbc.h"
#include "core.h"


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

int libgdbc_send_cmd(char* command) {
	if (send_command(&instance, command) == 0) {
	} else {
		return -1;
	}
}
