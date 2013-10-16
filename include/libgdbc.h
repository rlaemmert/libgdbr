#ifndef LIBGDBC_H
#define LIBGDBC_H

int libgdbc_init();
int libgdbc_cleanup();
int libgdbc_connect(char* server, int port);
int libgdbc_disconnect();

// Commands
int libgdbc_continue();
int libgdbc_send_cmd(char* command);

#endif
