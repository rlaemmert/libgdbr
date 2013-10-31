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

#include "utils.h"
#include "arch.h"

#define CMD_CONTINUE "c"
#define CMD_READREG  "g"
static int ARCH_X86_64 = 0;
static int ARCH_X86_32 = 1;

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
	int top; /*! Top of the message stack (index) */
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
	uint8_t architecture;
	register_t* registers;
} libgdbc_t;

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
 * \brief creates a new instance object (allocates buffers and such)
 * \param instance the "instance" of the current libgdbc session
 * \param architecture defines the architecure used (registersize, and such)
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int create_instance(libgdbc_t* instance, uint8_t architecture);
/*!
 * \brief deletes the given instance (frees buffers)
 * \param instance the "instance" of the current libgdbc session
 * \returns a failure code (currently -1) or 0 if call successfully
 */ 
int delete_instance(libgdbc_t* instance);

/*!
 * \brief connects to the gdbserver
 * \param instance the "instance" of the current libgdbc session
 * \param host defines the host in string representation
 * \param port of the connection
 * \returns a failure code (currently -1) or 0 if call successfully
 */ 
int connect_instance(libgdbc_t* instance, const char* host, int port);

/*!
 * \brief disconnects the instance
 * \param instance the "instance" of the current libgdbc session
 * \returns a failure code (currently -1) or 0 if call successfully
 */ 
int disconnect_instance(libgdbc_t* instance);

/*!
 * \brief dumps the whole message stack
 * \param instance the "instance" of the current libgdbc session
 * \returns a failure code (currently -1) or 0 if call successfully
 */ 
int dump_message_stack(libgdbc_t* instance);

int step_instance(libgdbc_t* instance);

/*!
 * \brief sends a 'm' packet to the gdbserver and reads the result
 * \param instance the "instance" of the current libgdbc session
 * \returns a failure code (currently -1) or 0 if call successfully
 */
int memread_instance(libgdbc_t* instance, uint64_t address, uint64_t len);

/*!
 * \brief sends a 'g' packet to the gdbserver and reads the result
 * \param instance the "instance" of the current libgdbc session
 * \returns a failure code (currently -1) or 0 if call successfully
 */ 
int regread_instance(libgdbc_t* instance);

/*!
 * \brief sends a 'c' packet to the gdbserver
 * \param instance the "instance" of the current libgdbc session
 * \returns a failure code (currently -1) or 0 if call successfully
 */ 
int continue_instance(libgdbc_t* instance);

#endif
