#include "libgdbc.h"
#include "utils.h"
#include <stdio.h> // TODO final remove after removing printf

/**
 * Function creates a new instance of libgdbc_t
 */
int libgdbc_create_instance(libgdbc_t* instance)
{
	memset(instance,0, sizeof(libgdbc_t));
	instance->send_buff = (char*) malloc(2500);
	instance->max_send_len = 2500;
	instance->read_buff = (char*) malloc(2500);
	instance->max_read_len = 2500;
	instance->connected = FALSE;
	return 0; 
}

/**
 * Function deletes existing instance
 * i.e. frees all allocated memory inside the instance
 * remember it does not free the instance itself
 */
int libgdbc_delete_instance(libgdbc_t* instance)
{
	int i;
	for (i = 0 ; i < instance->message_stack.top ; i++)
	{
		free(instance->message_stack.message_stack[i].msg);
	}
	free(instance->send_buff);
	instance->max_send_len = 0;
	free(instance->read_buff);
	instance->max_read_len = 0;
	return 0;
}

/**
 * Function connects the defined host:port kombination
 * to the existing gdbserver instance
 */
int libgdbc_connect(libgdbc_t* instance, const char* host, int port)
{
	int					fd;
	int					connected;
	struct protoent		*protocol;
	struct hostend		*hostaddr;
	struct sockaddr_in	socketaddr;
	
	protocol = getprotobyname("tcp");
	if (!protocol)
	{
		printf("Error prot\n");
		//TODO Error here
		return -1;
	}

	fd = socket( PF_INET, SOCK_STREAM, protocol->p_proto);
	if (fd == -1)
	{
		printf("Error sock\n");
		//TODO Error here
		return -1;
	}
	memset(&socketaddr, 0, sizeof(socketaddr));
	socketaddr.sin_family = AF_INET;
	socketaddr.sin_port = htons(port);
	hostaddr = gethostbyname(host);

	if (!hostaddr)
	{
		printf("Error host\n");
		//TODO Error here
		return -1;
	}
	
	connected = connect(fd, (struct sockaddr *) &socketaddr, sizeof(socketaddr));
	if (connected == -1)
	{
		printf("error conn\n");
		//TODO Error here
		return -1;
	}
	instance->fd = fd;
	instance->connected = TRUE;
	// TODO add config possibility here
	char* message = "qSupported:multiprocess+;qRelocInsn+";
	libgdbc_send_command(instance, message);
	return 0;
}

int libgdbc_disconnect(libgdbc_t* instance)
{
	// TODO Disconnect maybe send something to gdbserver
	close(instance->fd);
	instance->connected = FALSE;
	return 0;
}


int libgdbc_regread(libgdbc_t* instance)
{
	libgdbc_send_command(instance, libgdbc_read_registers_cmd);
	libgdbc_message_t* msg = &instance->message_stack.message_stack[instance->message_stack.top-1];
	printf("Msg: %s\n", msg->msg);
	
	int i = 0;
	printf("Len: %i\n", msg->len);
	for (i = 0;i<msg->len;i = i + 16)
	{
		uint8_t current[17];
		memcpy(current, msg->msg + i, 16);
		current[16] = '\0';
		uint64_t current_val = unpack_uint64_co (current, 16);
		printf("0x%016llx\n", current_val);
	}	
	return 0;
}

int libgdbc_continue(libgdbc_t* instance)
{
	return libgdbc_send_command(instance, libgdbc_continue_cmd);
}
