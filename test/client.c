#include <stdio.h>
#include "libgdbc.h"

int main() {
	libgdbc_init();

	if(libgdbc_connect("localhost", 1234) == -1) {
		printf("Error connecting to target\n");
		return 0;
	}
	//libgdbc_regread(&con);
	libgdbc_send_cmd("g");
	libgdbc_continue();
	libgdbc_disconnect();
	libgdbc_cleanup();
	return 0;
}
