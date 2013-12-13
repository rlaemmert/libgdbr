#include "libgdbr.h"
#include "core.h"
#include "packet.h"


int gdbr_init(libgdbr_t* instance, uint8_t architecture) {
	//memset(instance,0, sizeof(libgdbr_t));
	instance->send_buff = (char*) calloc(2500, sizeof(char));
	instance->max_send_len = 2500;
	instance->read_buff = (char*) calloc(2500, sizeof(char));
	instance->max_read_len = 2500;
	instance->connected = 0;
	instance->data_len = 0;
	instance->data = calloc(4096, sizeof(char));
	printf("0x%16llx\n", instance->data);
	instance->data_max = 4096;
	instance->architecture = architecture;
	//if (architecture == ARCH_X86_64) instance->registers = x86_64;
	//else if (architecture == ARCH_X86_32) instance->registers = x86_32;
	//else return -1;
	return 0; 
}


int gdbr_cleanup(libgdbr_t* instance) {
	int i;
	for (i = 0 ; i < instance->message_stack.count ; i++) {
		free(instance->message_stack.message_stack[i].msg);
	}
	printf("0x%16llx\n", instance->data);
	free(instance->data);
	free(instance->send_buff);
	instance->max_send_len = 0;
	free(instance->read_buff);
	instance->max_read_len = 0;
	return 0;
}


int gdbr_connect(libgdbr_t* instance, const char* host, int port) {
	int	fd;
	int	connected;
	struct protoent		*protocol;
	struct hostend		*hostaddr;
	struct sockaddr_in	socketaddr;
	
	protocol = getprotobyname("tcp");
	if (!protocol) {
		printf("Error prot\n");
		//TODO Error here
		return -1;
	}

	fd = socket( PF_INET, SOCK_STREAM, protocol->p_proto);
	if (fd == -1) {
		printf("Error sock\n");
		//TODO Error here
		return -1;
	}
	memset(&socketaddr, 0, sizeof(socketaddr));
	socketaddr.sin_family = AF_INET;
	socketaddr.sin_port = htons(port);
	hostaddr = gethostbyname(host);

	if (!hostaddr) {
		printf("Error host\n");
		//TODO Error here
		return -1;
	}
	
	connected = connect(fd, (struct sockaddr *) &socketaddr, sizeof(socketaddr));
	if (connected == -1) {
		printf("error conn\n");
		//TODO Error here
		return -1;
	}
	instance->fd = fd;
	instance->connected = 1;
	// TODO add config possibility here
	char* message = "qSupported:multiprocess+;qRelocInsn+";
	send_command(instance, message);
	return handle_connect(instance);
}


int gdbr_disconnect(libgdbr_t* instance) {
	// TODO Disconnect maybe send something to gdbserver
	close(instance->fd);
	instance->connected = 0;
	return 0;
}


int gdbr_read_registers(libgdbr_t* instance) {
	send_command(instance, CMD_READREG);
	return handle_g(instance);
}


int gdbr_read_memory(libgdbr_t* instance, uint64_t address, uint64_t len) {
	char command[255] = {};
	int ret = snprintf(command, 255, "m%016llx,%lld", address, len);
	send_command(instance, command);
	if (ret == -1) return ret;
	return handle_m(instance);
}


int gdbr_write_memory(libgdbr_t* instance, uint64_t address, char* data, uint64_t len) {
	char command[255] = {};
	snprintf(command, 255, "M%016llx,%lld:", address, len);
	int command_len = strlen(command);
	char* tmp = calloc(command_len + (len * 2), sizeof(char));
	memcpy(tmp, command, command_len);
	pack_hex(data, len, (tmp + command_len));
	send_command(instance, tmp);
	free(tmp);
	return 0;
}


int gdbr_continue(libgdbr_t* instance) {
	return send_command(instance, CMD_CONTINUE);
}


int gdbr_send_command(libgdbr_t* instance, char* command) {
	char* cmd = calloc(strlen(command) * 2, sizeof(char));
	char* txt_command = "qRcmd,";
	strcpy(cmd, txt_command);
	pack_hex(command, strlen(command), (cmd + strlen(txt_command)+1));
	int ret = send_command(instance, cmd);
	free(cmd);
	if (ret == -1) return ret;
	return handle_cmd(instance);
}	


int dump_message_stack(libgdbr_t* instance) {
	libgdbr_message_t* current = &instance->message_stack.message_stack[0];
	while(current <= &instance->message_stack.message_stack[instance->message_stack.count	- 1]) {
		printf("Msg: %s\n", current->msg);
		current++;
	}
}


int send_command(libgdbr_t* instance, char* command) {
	uint8_t checksum = cmd_checksum(command);
	int ret = snprintf(instance->send_buff, instance->max_send_len, "$%s#%.2x", command, checksum);
	if (ret == -1) {
		return -1;
	}
	instance->data_len = ret;
	return send_packet(instance);
}


int send_packet(libgdbr_t* instance) {
	if (!instance) {
		// TODO corect error handling here
		printf("Initialize libgdbr_t first\n");
		return -1;
	}
	printf("Sending: %s\n", instance->send_buff);
	int ret = send(instance->fd, instance->send_buff, instance->data_len, 0);
	read_packet(instance); // TODO handle the size of the answer in some kind?
	return 0;
}


int read_packet(libgdbr_t* instance) {
	if (!instance) {
		// TODO correct error handling here
		printf("Initialize libgdbr_t first\n");
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
	while (result > 0) {
		FD_ZERO(&readset);
		FD_SET(instance->fd, &readset);
		result = select(instance->fd + 1, &readset, NULL, NULL, &tv);
		if (result > 0) {
			if (FD_ISSET(instance->fd, &readset)) {
				ret = recv(instance->fd, (instance->read_buff + current_size), instance->max_read_len, 0);
				current_size += ret;
			}
		}
	}
	instance->data_len = current_size;
	parse_packet(instance);
	return ret;
}
