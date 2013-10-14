#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include "libgdbc.h"

/**
 * Calculates the right checksum for a given gdb command
 *  command - the command to calculate the checksum from
 *  @returns 8bit checksum
 */ 


typedef struct libgdbc_receive_queue_t
{
	uint8_t acks; // every + (ack) will increase the current counter
	char** message_stack;	// message stack that contains all unhandled messages
} libgdbc_receive_queue_t;

uint8_t libgdbc_cmd_checksum(const char* command);
int libgdbc_send_command(libgdbc_t* instance, char* command);
int libgdbc_send_packet(libgdbc_t* instance);
int libgdbc_read_packet(libgdbc_t* instance);
// TODO decide if another package is needet
int hex2int(int ch);
uint64_t unpack_uint64 (char *buff, int len);
uint64_t unpack_uint64_co (char* buff, int len);

#endif
