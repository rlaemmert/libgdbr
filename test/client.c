#include <stdio.h>
#include "libgdbc.h"

int main() {
	libgdbc_t gdb;
	gdbc_init(&gdb, 0);
	char buffer[2500];
	if(gdbc_connect(&gdb, "localhost", 1234) == -1) {
		printf("Error connecting to target\n");
		return 0;
	}
	gdbc_read_registers(&gdb);

	//do {
	//	fgets(buffer, sizeof(buffer) - 1, stdin);
	//	libgdbc_send_cmd(buffer);
	//} while (strncmp("quit", buffer, 3));
	gdbc_read_memory(&gdb, 0xfffffa8004ac2010, 300);
	gdbc_continue(&gdb);
	gdbc_disconnect(&gdb);
	gdbc_cleanup(&gdb);
	return 0;
}
