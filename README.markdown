## libgdbr - Library that handles all remote communication with gdb
```
#include <stdio.h>
#include "libgdbr.h"

int main() {
	libgdbr_t gdb = {};
	gdbr_init(&gdb);
	gdbr_set_architecture(&gdb, X86_64);
	if(gdbr_connect(&gdb, "localhost", 1234) == -1) {
		printf("Error connecting to target\n");
		return 0;
	}

	gdbr_read_registers(&gdb);
	// gdb.data && data.data_len (contains the result)
	gdbr_send_command(&gdb, "info mem");
	gdbr_disconnect(&gdb);
	gdbr_cleanup(&gdb);
	return 0;
}
```

