/*! \file */
#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "libgdbc.h"
#include "utils.h"
#include "arch.h"

#define CMD_CONTINUE "c"
#define CMD_READREG  "g"
static int ARCH_X86_64 = 0;
static int ARCH_X86_32 = 1;


/*!
 * \brief Function sends a command to the gdbserver 
 * \param instance the "instance" of the current libgdbc session
 * \param command the command that will be sent
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int send_command(libgdbc_t* instance, char* command);

/*!
 * \brief sends a packet sends a packet to the established connection
 * \param instance the "instance" of the current libgdbc session
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int send_packet(libgdbc_t* instance);

/*!
 * \brief Function reads data from the established connection
 * \param instance the "instance" of the current libgdbc session
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int read_packet(libgdbc_t* instance);

/*!
 * \brief dumps the whole message stack
 * \param instance the "instance" of the current libgdbc session
 * \returns a failure code (currently -1) or 0 if call successfully
 */ 
int dump_message_stack(libgdbc_t* instance);

#endif
