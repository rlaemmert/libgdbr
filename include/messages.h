/**
 * See Appendix E in the gdb manual (GDB Remote Serial Protocol)
 * Packets look following: $ starts a command/packet, the end is indicated
 * with # and a final checksum
 * $<command>#<checksum>
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include "core.h"


int handle_g(libgdbc_t* instance);

#endif
