#ifndef LIBGDBC_H
#define LIBGDBC_H

#include <stdint.h>

int libgdbc_init();
int libgdbc_cleanup();
int libgdbc_connect(char* server, int port);
int libgdbc_disconnect();

// Commands
int libgdbc_continue();
int libgdbc_read_registers();
int libgdbc_read_memory(uint64_t address, uint64_t len);
int libgdbc_send_cmd(char* command);

#endif
