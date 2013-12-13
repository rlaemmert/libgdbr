/*! \file */
/**
 * See Appendix E in the gdb manual (GDB Remote Serial Protocol)
 * Packets look following: $ starts a command/packet, the end is indicated
 * with # and a final checksum
 * $<command>#<checksum>
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include "libgdbr.h"

int handle_g(libgdbr_t* instance);
int handle_m(libgdbr_t* instance);
int handle_cmd(libgdbr_t* instance);
int handle_connect(libgdbr_t* instance);

#endif
