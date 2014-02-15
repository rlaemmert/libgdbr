# libgdbr 
#### Library that handles all remote communication with gdb
This project starts as help for radare2 to implement fully
gdb remote support. Development is still in progress and i try
to add all features. 

#### Working features:
* read/write registers
* read/write memory
* step/continue
* send gdb commands like 'info mem' or 'info registers'

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
