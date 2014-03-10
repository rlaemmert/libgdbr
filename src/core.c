/* libgdbr - LGPL - Copyright 2014 - defragger */

#include "libgdbr.h"
#include "core.h"
#include "packet.h"
#include "messages.h"

int r_gdb_init(libgdbr_t* g) {
	memset(g ,0 , sizeof(libgdbr_t));
	g->send_buff = (char*) calloc(2500, sizeof(char));
	g->send_len = 0;
	g->send_max = 2500;
	g->read_buff = (char*) calloc(4096, sizeof(char));
	g->read_len = 0;
	g->read_max = 4096;
	g->connected = 0;
	g->data_len = 0;
	g->data = calloc(4096, sizeof(char));
	g->data_max = 4096;
	return 0; 
}

int r_gdb_set_architecture(libgdbr_t* g, uint8_t architecture) {
	g->architecture = architecture;
	switch (architecture) {
		case ARCH_X86_32:
			g->registers = x86_32;
			break;
		case ARCH_X86_64:
			g->registers = x86_64;
			break;
		default:
			printf("Error unknown architecture set\n");
	}
	return 0;
}

int r_gdb_cleanup(libgdbr_t* g) {
	free(g->data);
	free(g->send_buff);
	g->send_len = 0;
	free(g->read_buff);
	g->read_len = 0;
	return 0;
}

int r_gdb_connect(libgdbr_t* g, const char* host, int port) {
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
	g->fd = fd;
	g->connected = 1;
	// TODO add config possibility here
	char* message = "qSupported:multiprocess+;qRelocInsn+";
	send_command(g, message);
	read_packet(g);
	return handle_connect(g);
}

int r_gdb_disconnect(libgdbr_t* g) {
	// TODO Disconnect maybe send something to gdbserver
	close(g->fd);
	g->connected = 0;
	return 0;
}

int r_gdb_read_registers(libgdbr_t* g) {
	send_command(g, CMD_READREGS);
	int read_len = read_packet(g);
	if ( read_len > 0) {
		parse_packet(g, 0);
		return handle_g(g);
	}
	return -1;
}

int r_gdb_read_memory(libgdbr_t* g, uint64_t address, uint64_t len) {
	char command[255] = {};
	int ret = snprintf(command, 255, "%s%016lx,%ld", CMD_READMEM, address, len);
	if (ret < 0) return ret;
	send_command(g, command);

	int read_len = read_packet(g);
	if (read_len > 0) { 
		parse_packet(g, 0);
		return handle_m(g);
	}
	return -1;
}

int r_gdb_write_memory(libgdbr_t* g, uint64_t address, char* data, uint64_t len) {
	char command[255] = {};
	int command_len = snprintf(command, 255, "%s%016lx,%ld:", CMD_WRITEMEM, address, len);
	char* tmp = calloc(command_len + (len * 2), sizeof(char));
	memcpy(tmp, command, command_len);
	pack_hex(data, len, (tmp + command_len));
	send_command(g, tmp);
	free(tmp);

	int read_len = read_packet(g);
	if (read_len > 0) {
		parse_packet(g, 0);
		return 0;
	}
	return -1;
}

int r_gdb_step(libgdbr_t* g, int thread_id) {
	return send_vcont(g, CMD_C_STEP, thread_id);
}

int r_gdb_continue(libgdbr_t* g, int thread_id) {
	return send_vcont(g, CMD_C_CONT, thread_id);
}

int r_gdb_send_command(libgdbr_t* g, char* command) {
	char* cmd = calloc((strlen(command) * 2 + strlen(CMD_QRCMD) + 2), sizeof(char));
	strcpy(cmd, CMD_QRCMD);
	pack_hex(command, strlen(command), (cmd + strlen(CMD_QRCMD)));
	int ret = send_command(g, cmd);
	free(cmd);
	if (ret < 0) return ret;

	int read_len = read_packet(g);
	if (read_len > 0) {
		parse_packet(g, 1);
		return handle_cmd(g);
	}
	return -1;
}	

int r_gdb_write_bin_registers(libgdbr_t* g, char* registers) {
	r_gdb_read_registers(g);

	uint64_t buffer_size = g->data_len * 2 + 8;
	char* command = calloc(buffer_size, sizeof(char));
	snprintf(command, buffer_size, "%s", CMD_WRITEREGS);
	pack_hex(g->data, g->data_len, command+1);
	send_command(g, command);
	read_packet(g);
	free(command);
	handle_G(g);
	return 0;
}

int r_gdb_write_registers(libgdbr_t* g, char* registers) {
	int ret = 0;
	// read current register set
	r_gdb_read_registers(g);

	int len = strlen(registers);
	char* buff = calloc(len, sizeof(char));
	memcpy(buff, registers, len);
	char* reg = strtok(buff, ",");
	while ( reg != NULL ) {
		char* name_end = strchr(reg, '=');
		if (name_end == NULL) {
			printf("Malformed argument: %s\n", reg);
			free(buff);
			return -1;
		}
		*name_end = '\0'; // change '=' to '\0'

		// time to find the current register
		int i = 0;
		while ( g->registers[i].size > 0) {
			if (strcmp(g->registers[i].name, reg) == 0) {

				uint64_t register_size = g->registers[i].size;
				uint64_t offset = g->registers[i].offset;

				char* value = calloc(register_size * 2, sizeof(char));

				memset(value, '0', register_size * 2);
								
				name_end++; 
				// be able to take hex with and without 0x
				if (name_end[1] == 'x' || name_end[1] == 'X') name_end += 2;
				int val_len = strlen(name_end); // size of the rest
				strcpy(value+(register_size * 2 - val_len), name_end);

				int x = 0;
				while (x < register_size) {
					g->data[offset + register_size - x - 1] = hex2char(&value[x * 2]);
					x++;
				}
				free(value);
			}
			i++;
		}
		reg = strtok(NULL, " ,");
	}

	free(buff);

	uint64_t buffer_size = g->data_len * 2 + 8;
	char* command = calloc(buffer_size, sizeof(char));
	snprintf(command, buffer_size, "%s", CMD_WRITEREGS);
	pack_hex(g->data, g->data_len, command+1);
	send_command(g, command);
	read_packet(g);
	free(command);
	handle_G(g);
	return 0;
}

int test_command(libgdbr_t* g, char* command) {
	send_command(g, command);
	read_packet(g);
	hexdump(g->read_buff, g->data_len, 0);
	return 0;
}

int send_vcont(libgdbr_t* g, char* command, int thread_id) {
	char tmp[255] = {};
	int ret = snprintf(tmp, 255, "%s;%s:%x", CMD_C, command, thread_id);
	if (ret < 0) return ret;
	send_command(g, tmp);
	int read_len = read_packet(g);
	if (read_len > 0) { 
		parse_packet(g, 0);
		return handle_cont(g);
	}
	return 0;
}

int set_bp(libgdbr_t* g, uint64_t address, char* conditions, enum Breakpoint type) {
	char tmp[255] = {};
	int ret = 0;
	switch(type) {
		case BREAKPOINT:
			ret = snprintf(tmp, 255, "%s,%llx,1", CMD_BP, address);
			break;
		case HARDWARE_BREAKPOINT:
			ret = snprintf(tmp, 255, "%s,%llx,1", CMD_HBP, address);
			break;
		case WRITE_WATCHPOINT:
			break;
		case READ_WATCHPOINT:
			break;
		case ACCESS_WATCHPOINT:
			break;
		default:
			break;
	}
	if (ret < 0) return ret;
	send_command(g, tmp);

	int read_len = read_packet(g);
	if (read_len > 0) {
		parse_packet(g, 0);
		return handle_setbp(g);
	}
	return 0;
}

int r_gdb_set_bp(libgdbr_t* g, uint64_t address, char* conditions) {
	return set_bp(g, address, conditions, BREAKPOINT);
}

int r_gdb_set_hwbp(libgdbr_t* g, uint64_t address, char* conditions) {
	return set_bp(g, address, conditions, HARDWARE_BREAKPOINT);
}

int r_gdb_remove_bp(libgdbr_t* g, uint64_t address) {
	return remove_bp(g, address, BREAKPOINT);
}

int r_gdb_remove_hwbp(libgdbr_t* g, uint64_t address) {
	return remove_bp(g, address, HARDWARE_BREAKPOINT);
}

int remove_bp(libgdbr_t* g, uint64_t address, char* conditions, enum Breakpoint type) {
	char tmp[255] = {};
	int ret = 0;
	switch(type) {
		case BREAKPOINT:
			ret = snprintf(tmp, 255, "%s,%llx,1", CMD_RBP, address);
			break;
		case HARDWARE_BREAKPOINT:
			ret = snprintf(tmp, 255, "%s,%llx,1", CMD_RHBP, address);
			break;
		case WRITE_WATCHPOINT:
			break;
		case READ_WATCHPOINT:
			break;
		case ACCESS_WATCHPOINT:
			break;
		default:
			break;
	}
	if (ret < 0) return ret;
	send_command(g, tmp);

	int read_len = read_packet(g);
	if (read_len > 0) {
		parse_packet(g, 0);
		return handle_removebp(g);
	}
	return 0;
}

int send_ack(libgdbr_t* g) {
	g->send_buff[0] = '+';
	g->send_len = 1;
	send_packet(g);
	return 0;
}

int send_command(libgdbr_t* g, char* command) {
	uint8_t checksum = cmd_checksum(command);
	int ret = snprintf(g->send_buff, g->send_max, "$%s#%.2x", command, checksum);
	if (ret < 0) {
		return ret;
	}
	g->send_len = ret;
	return send_packet(g);
}

