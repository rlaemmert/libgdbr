#include "packet.h"


char get_next_token(parsing_object_t* current) {
	printf("%c ", current->buffer[current->position]);
	return current->buffer[current->position++];
}


void handle_escape(parsing_object_t* current) {
	if (current->position >= current->length) return;
	char token = get_next_token(current);
	if (token == '}') handle_data(current);
	else handle_escape(current);
}


void handle_chk(parsing_object_t* current) {
	if (current->position >= current->length) return;
	char checksum[2];
	checksum[0] = get_next_token(current);
	checksum[1] = get_next_token(current);
	checksum[2] = '\0';
	current->checksum = (uint8_t) strtol(checksum, NULL, 16);
}


void handle_data(parsing_object_t* current) {
	if (current->position >= current->length) return;
	char token = get_next_token(current);
	if (token == '#') {
		current->end = current->position - 1; // subtract 2 cause of # itself and the incremented position after getNextToken
		handle_chk(current);
	}
	else if	(token == '{') handle_escape(current);
	else handle_data(current);
}


void handle_packet(parsing_object_t* current) {
	if(current->position >= current->length) return;

	char token = get_next_token(current);
	if (token == '$') {
		current->start = current->position;
		handle_data(current);
	}
	else if	(token == '+') {
		printf("KLekse\n");
		current->acks++;
		handle_packet(current);
	}
}

// TODO ugly implementation (128 max message stack and pop_message does not care about the allocated buffer in push_message
int push_message(libgdbr_t* instance, parsing_object_t* parsed) {
	if (instance->message_stack.count >= 128) {
		return -1;
	}
	instance->message_stack.count++; // increment the count of our message stack
	int index = instance->message_stack.count - 1;
	libgdbr_message_t* message = &instance->message_stack.message_stack[index];
	message->msg = (char*) calloc((parsed->end - parsed->start) + 1, sizeof(char));
	ssize_t len = (parsed->end - parsed->start);
	memcpy(message->msg, parsed->buffer + parsed->start, len);
	message->chk = parsed->checksum;
	message->len = len;
	return 0;
}


char* pop_message(libgdbr_t* instance) {
	if (instance->message_stack.count <= 0) {
		return -1;
	}
	int index = --instance->message_stack.count;
	char* msg = instance->message_stack.message_stack[index].msg;
	if (instance->message_stack.message_stack[index].chk == cmd_checksum(msg)) {
		return msg;
	}
	return -1;
}

int parse_packet(libgdbr_t* instance) {
	parsing_object_t new;
	memset(&new, 0, sizeof(parsing_object_t));
	new.length = instance->data_len;
	printf("len: %i\n", new.length);
	new.buffer = instance->read_buff;
	while(new.position < new.length) {
		handle_packet(&new);
		push_message(instance, &new);
	}
	// TODO check if its possible that gdbserver sends multiple acks in 1 packet
	instance->acks += new.acks;
	return 0;
}
