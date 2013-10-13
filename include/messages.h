/**
 * See Appendix E in the gdb manual (GDB Remote Serial Protocol)
 * Packets look following: $ starts a command/packet, the end is indicated
 * with # and a final checksum
 * $<command>#<checksum>
 */
static char* libgdbc_continue_cmd				= "c";
static char* libgdbc_read_registers_cmd	= "g";

