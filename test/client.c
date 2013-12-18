#include <stdio.h>
#include "libgdbr.h"

int main() {
	libgdbr_t gdb = {};
	gdbr_init(&gdb, 0);
	//char buffer[2500];
	if(gdbr_connect(&gdb, "localhost", 1234) == -1) {
		printf("Error connecting to target\n");
		return 0;
	}
	gdbr_read_registers(&gdb);

	//do {
	//	fgets(buffer, sizeof(buffer) - 1, stdin);
	//	libgdbr_send_cmd(buffer);
	//} while (strncmp("quit", buffer, 3));
	gdbr_write_memory(&gdb, 0xfffff8800174328f, "Hallo", 5);

	gdbr_read_memory(&gdb, 0xfffff8800174328f, 300);
	gdbr_send_command(&gdb, "info registers");
	//gdbr_send_command(&gdb, "info mem");
	gdbr_continue(&gdb);
	gdbr_disconnect(&gdb);
	gdbr_cleanup(&gdb);
	return 0;
}
