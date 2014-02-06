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

#include "libgdbr.h"
#include "utils.h"
#include "arch.h"

#define CMD_CONTINUE	"c"
#define CMD_READREGS	"g"
#define CMD_WRITEREGS	"G"
#define CMD_WRITEMEM	"M"
#define CMD_READMEM		"m"


/*!
 * \brief Function sends a command to the gdbserver 
 * \param instance the "instance" of the current libgdbr session
 * \param command the command that will be sent
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int send_command(libgdbr_t* instance, char* command);

#endif
