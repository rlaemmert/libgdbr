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
		current->acks++;
		handle_packet(current);
	}
}


int parse_packet(libgdbr_t* instance) {
	parsing_object_t new;
	memset(&new, 0, sizeof(parsing_object_t));
	new.length = instance->data_len;
	new.buffer = instance->read_buff;
	uint64_t target_pos = 0;
	while(new.position < new.length) {
		handle_packet(&new);
		uint64_t current_size = new.end - new.start;
		if ( instance->data_max <= (instance->data_len + current_size)) {
			instance->data = realloc(instance->data, instance->data_len + current_size + 1);
			instance->data_len += current_size;
			instance->data_max += current_size;
		}
		memcpy(instance->data + target_pos , new.buffer + new.start, current_size);
		char* tmp = calloc(current_size, sizeof(char));
		memcpy(tmp, new.buffer + new.start, current_size);
		printf("LINE: %s\n", tmp);
		target_pos += current_size;
	}
	instance->data[instance->data_len] = '\0';
	printf("Result: %s\n", instance->data);
	// TODO check if its possible that gdbserver sends multiple acks in 1 packet
	return 0;
}
