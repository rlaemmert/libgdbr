#include <stdio.h>
#include "libgdbc.h"

int main() {
	libgdbc_init();
	char buffer[2500];
	if(libgdbc_connect("localhost", 1234) == -1) {
		printf("Error connecting to target\n");
		return 0;
	}
	libgdbc_read_registers();

	//do {
	//	fgets(buffer, sizeof(buffer) - 1, stdin);
	//	libgdbc_send_cmd(buffer);
	//} while (strncmp("quit", buffer, 3));
	libgdbc_read_memory(0xfffff800fa96e5c8, 10);
	libgdbc_continue();
	libgdbc_disconnect();
	libgdbc_cleanup();
	return 0;
}
