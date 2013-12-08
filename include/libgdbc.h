/*! \file */
#ifndef LIBGDBC_H
#define LIBGDBC_H

#include <stdint.h>
#include <unistd.h>

#include "arch.h"


/*! 
 * Structure that saves a gdb message
 */
typedef struct libgdbc_message_t
{
	ssize_t len; /*! Len of the message */
	char* msg;	/*! Pointer to the buffer that contains the message */
	uint8_t chk;	/*! Cheksum of the current message read from the packet */
} libgdbc_message_t;


/*! 
 * Message stack
 */
typedef struct libgdbc_message_stack_t
{
	int count; 
	libgdbc_message_t message_stack[128];	/*! Message stack itself (just a trivial array with 128 places */
} libgdbc_message_stack_t;


/*! 
 * Core "object" that saves
 * the instance of the lib
 */
typedef struct libgdbc_t 
{
	char* send_buff; // defines a buffer for reading and sending stuff
	ssize_t max_send_len; // definses the maximal len for the given buffer

	char* read_buff;
	ssize_t max_read_len;

	libgdbc_message_stack_t message_stack; // keeps the unhandled messages
	// is already handled (i.e. already send or ...)
	int fd; // Filedescriptor // TODO add r_socket stuff from radare
	int connected;
	int acks;
	uint8_t* data;
	ssize_t data_len;
	ssize_t data_max;
	uint8_t architecture;
	registers_t* registers;
} libgdbc_t;

/*!
 * \brief Function initializes the libgdbc lib
 * \param architecture defines the architecure used (registersize, and such)
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int gdbc_init(libgdbc_t* instance, uint8_t architecture);

/*!
 * \brief frees all buffers and cleans the libgdbc instance stuff
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int gdbc_cleanup(libgdbc_t* instance);

/*!
 * \brief Function connects to a gdbserver instance
 * \param server string that represents the host
 * \param number that represents the port
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int gdbc_connect(libgdbc_t* instance, const char* server, int port);

/*!
 * \brief disconnects the lib
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int gdbc_disconnect(libgdbc_t* instance);

// Commands
int gdbc_continue(libgdbc_t* instance);
int gdbc_read_registers(libgdbc_t* instance);
int gdbc_read_memory(libgdbc_t* instance, uint64_t address, uint64_t len);
int gdbc_send_cmd(libgdbc_t* instance, char* command);

#endif
