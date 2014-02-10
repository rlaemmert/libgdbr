#include <stdio.h>
#include "libgdbr.h"

int main() {
	libgdbr_t gdb = {};
	gdbr_init(&gdb);
	gdbr_set_architecture(&gdb, X86_64);
	//char buffer[2500];
	if(gdbr_connect(&gdb, "localhost", 1234) == -1) {
		printf("Error connecting to target\n");
		return 0;
	}
	//gdbr_read_registers(&gdb);

	char buffer[255] = {};
	//do {
	//	fgets(buffer, sizeof(buffer) - 2, stdin);
	//	test_command(&gdb, buffer);
	//} while (strncmp("quit", buffer, 3));
	//gdbr_write_memory(&gdb, 0xfffff8800174328f, "Hallo", 5);
	gdbr_read_registers(&gdb);

	//gdbr_read_memory(&gdb, 0xfffff880017435f, 800);
	//gdbr_send_command(&gdb, "info registers");
	gdbr_write_registers(&gdb, "rax=fffff8002a5145c8,xmm0=1337133713371337,xmm1=0x123123123123123");
	//gdbr_send_command(&gdb, "info mem");
	//gdbr_continue(&gdb);
	gdbr_disconnect(&gdb);
	gdbr_cleanup(&gdb);
	return 0;
}

