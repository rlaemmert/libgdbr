#include "utils.h"
#include "packet.h"

/**
 * This function creates the checksum
 * for the given command
 * command : is used to calculate the checksum
 */
uint8_t libgdbc_cmd_checksum(const char* command)
{
	int i, len;
	uint8_t sum = 0;
	len = strlen(command);
	for(i = 0; i < len ; i++)
	{
			sum += command[i];	
	}
	return sum;
}

/**
 * This function sends the given command to the gdb server
 * it creates the needet checksum and creates the packet
 * i.e. command = 'g' will end in $g#67
 * instance : instance that defines the current gdb session
 * command	: defines the given command
 */
int libgdbc_send_command(libgdbc_t* instance, char* command)
{
	uint8_t checksum = libgdbc_cmd_checksum(command);
	int ret = snprintf(instance->send_buff, instance->max_send_len, "$%s#%.2x", command, checksum);
	if(ret == -1)
	{
		return -1;
	}
	instance->data_len = ret;
	return libgdbc_send_packet(instance);
}

/**
 * This function sends the packet that lays
 * in instance->buff and checks the ack
 * from the server 
 * instance : defines the current gdb session
 */
int libgdbc_send_packet(libgdbc_t* instance)
{
	if(!instance)
	{
		// TODO corect error handling here
		printf("Initialize libgdbc_t first\n");
		return -1;
	}
	printf("Sending: %s\n", instance->send_buff);
	int ret = send(instance->fd, instance->send_buff, instance->data_len, 0);
	int acks = instance->acks;
	libgdbc_read_packet(instance);
	if(instance->acks > acks)
	{
		instance->acks--;
		return TRUE;
	}
	printf("FALSE!!!!!!!\n");
	return FALSE;
}


int libgdbc_read_packet(libgdbc_t* instance)
{
	if(!instance)
	{
		// TODO correct error handling here
		printf("Initialize libgdbc_t first\n");
		return -1;
	}
	int ret = 0;
	int current_size = 0;
	int i;
	fd_set readset;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	int result = 1;
	while(result > 0)
	{
		FD_ZERO(&readset);
		FD_SET(instance->fd, &readset);
		result = select(instance->fd + 1, &readset, NULL, NULL, &tv);
		if(result > 0)
		{
			if (FD_ISSET(instance->fd, &readset))
			{
				ret = recv(instance->fd, instance->read_buff + current_size, instance->read_buff, 0);
				current_size += ret;
			}
		}
	}
	instance->data_len = current_size;
	libgdbc_parse_packet(instance);
	return ret;
}
