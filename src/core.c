#include "libgdbr.h"
#include "core.h"
#include "packet.h"
#include "messages.h"


int gdbr_init(libgdbr_t* instance, uint8_t architecture) {
	memset(instance,0, sizeof(libgdbr_t));
	instance->send_buff = (char*) calloc(2500, sizeof(char));
	instance->send_len = 2500;
	instance->read_buff = (char*) calloc(2500, sizeof(char));
	instance->read_len = 2500;
	instance->max_read_size = 2500;
	instance->connected = 0;
	instance->data_len = 0;
	instance->data = calloc(4096, sizeof(char));
	instance->data_max = 4096;
	instance->architecture = architecture;
	return 0; 
}


int gdbr_cleanup(libgdbr_t* instance) {
	free(instance->data);
	free(instance->send_buff);
	instance->send_len = 0;
	free(instance->read_buff);
	instance->read_len = 0;
	return 0;
}


int gdbr_connect(libgdbr_t* instance, const char* host, int port) {
	int	fd;
	int	connected;
	struct protoent		*protocol;
	struct hostent		*hostaddr;
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
	hostaddr = (struct hostent *)gethostbyname(host);

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
	read_packet(instance);
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
	int read_len = read_packet(instance);
	if ( read_len > 0) {
		parse_packet(instance, 0);
		return handle_g(instance);
	}
	return -1;
}


int gdbr_read_memory(libgdbr_t* instance, uint64_t address, uint64_t len) {
	char command[255] = {};
	int ret = snprintf(command, 255, "m%016lx,%ld", address, len);
	send_command(instance, command);
	if (ret < 0) return ret;

	int read_len = read_packet(instance);
	if (read_len > 0) { 
		parse_packet(instance, 0);
		return handle_m(instance);
	}
	return -1;
}


int gdbr_write_memory(libgdbr_t* instance, uint64_t address, char* data, uint64_t len) {
	char command[255] = {};
	snprintf(command, 255, "M%016lx,%ld:", address, len);
	int command_len = strlen(command);
	char* tmp = calloc(command_len + (len * 2), sizeof(char));
	memcpy(tmp, command, command_len);
	pack_hex(data, len, (tmp + command_len));
	send_command(instance, tmp);
	free(tmp);

	int read_len = read_packet(instance);
	if (read_len > 0) {
		parse_packet(instance, 0);
		return 0;
	}
	return -1;
}


int gdbr_continue(libgdbr_t* instance) {
	return send_command(instance, CMD_CONTINUE);
}


int gdbr_send_command(libgdbr_t* instance, char* command) {
	char* txt_command = "qRcmd,";
	char* cmd = calloc((strlen(command) * 2 + strlen(txt_command) + 2), sizeof(char));
	strcpy(cmd, txt_command);
	pack_hex(command, strlen(command), (cmd + strlen(txt_command)));
	int ret = send_command(instance, cmd);
	free(cmd);
	if (ret < 0) return ret;

	int read_len = read_packet(instance);
	if (read_len > 0) {
		parse_packet(instance, 1);
		return handle_cmd(instance);
	}
	return -1;
}	


int send_command(libgdbr_t* instance, char* command) {
	uint8_t checksum = cmd_checksum(command);
	int ret = snprintf(instance->send_buff, instance->send_len, "$%s#%.2x", command, checksum);
	if (ret < 0) {
		return ret;
	}
	instance->data_len = ret;
	return send_packet(instance);
}

