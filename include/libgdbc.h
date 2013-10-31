/*! \file */
#ifndef LIBGDBC_H
#define LIBGDBC_H

#include <stdint.h>


/*!
 * \brief Function initializes the libgdbc lib
 * \param architecture defines the architecure used (registersize, and such)
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int libgdbc_init(uint8_t architecture);

/*!
 * \brief frees all buffers and cleans the libgdbc instance stuff
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int libgdbc_cleanup();

/*!
 * \brief Function connects to a gdbserver instance
 * \param server string that represents the host
 * \param number that represents the port
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int libgdbc_connect(char* server, int port);

/*!
 * \brief disconnects the lib
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int libgdbc_disconnect();

// Commands
int libgdbc_continue();
int libgdbc_read_registers();
int libgdbc_read_memory(uint64_t address, uint64_t len);
int libgdbc_send_cmd(char* command);

#endif
