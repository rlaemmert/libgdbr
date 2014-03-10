#include <string.h>
#include <stdio.h>
#include "libgdbr.h"

int main() {
	libgdbr_t gdb = {};
	r_gdb_init(&gdb);
	r_gdb_set_architecture(&gdb, X86_64);
	//char buffer[2500];
	if(r_gdb_connect(&gdb, "localhost", 1234) == -1) {
		printf("Error connecting to target\n");
		return 0;
	}
	//r_gdb_read_registers(&gdb);

	char buffer[255] = {};
	//do {
	//	fgets(buffer, sizeof(buffer) - 2, stdin);
	//	test_command(&gdb, buffer);
	//} while (strncmp("quit", buffer, 3));
	//.r_gdb_read_registers(&gdb);
	//r_gdb_write_memory(&gdb, 0x000000797e51c850, "Hallo", 5);
//	r_gdb_write_bin_registers(&gdb, "");
	//r_gdb_read_memory(&gdb, 0x000000797e51c850, 800);
	//r_gdb_send_command(&gdb, "info registers");
	//r_gdb_write_registers(&gdb, "rax=fffff8002a5145c8,xmm0=1337133713371337,xmm1=0x123123123123123");
	r_gdb_send_command(&gdb, "info registers");
	char* pos = strstr(gdb.data, "GS =002b");
	pos += 9;
	char* end = strchr(pos, ' ');
	uint64_t gs = unpack_uint64(pos, (end-pos));

	char* tmp = (char*)gs;
	char* teb = *(char**)(gs+0x30);
	printf("gs: 0x%llx\n", tmp);
	printf("teb: 0x%llx\n", teb);
	//r_gdb_continue(&gdb, -1);
	//r_gdb_step(&gdb, -1);
	r_gdb_disconnect(&gdb);
	r_gdb_cleanup(&gdb);
	return 0;
}

