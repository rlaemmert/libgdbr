#include "packet.h"

#include <stdio.h>

char get_next_token(parsing_object_t* current)
{
	printf("%c ", current->buffer[current->position]);
	return current->buffer[current->position++];
}

void handle_escape(parsing_object_t* current)
{
	if (current->position >= current->length) return;
	char token = get_next_token(current);
	if (token == '}') handle_data(current);
	else handle_escape(current);
}

void handle_chk(parsing_object_t* current)
{
	if (current->position >= current->length) return;
	char checksum[2];
	checksum[0] = get_next_token(current);
	checksum[1] = get_next_token(current);
	checksum[2] = '\0';
	current->checksum = (uint8_t) strtol(checksum, &checksum[1], 16);
	printf("Checksum: %x\n", current->checksum);
}

void handle_data(parsing_object_t* current)
{
	if (current->position >= current->length) return;
	char token = get_next_token(current);
	if (token == '#')
	{
		printf("End\n");
		current->end = current->position - 1; // subtract 2 cause of # itself and the incremented position after getNextToken
		handle_chk(current);
	}
	else if	(token == '{') handle_escape(current);
	else handle_data(current);
}

void handle_packet(parsing_object_t* current)
{
	if(current->position >= current->length) return;

	char token = get_next_token(current);
	if		(token == '$')
	{
		printf("Start\n");
		current->start = current->position;
		handle_data(current);
	}
	else if	(token == '+')
	{
		printf("Ack\n");
		handle_packet(current);
	}
}

int libgdbc_push_message(libgdbc_t* instance, parsing_object_t* parsed)
{
	libgdbc_message_t* message = &instance->message_stack.message_stack[instance->message_stack.top++];
	message->msg = (char*) calloc((parsed->end - parsed->start) + 1, sizeof(char));
	ssize_t len = (parsed->end - parsed->start);
	memcpy(message->msg, parsed->buffer + parsed->start, len);
	message->chk = parsed->checksum;
	message->len = len;
	return 0;
}

int libgdbc_parse_packet(libgdbc_t* instance)
{
	parsing_object_t new;
	memset(&new, 0, sizeof(parsing_object_t));
	new.length = instance->data_len;
	new.buffer = instance->read_buff;
	int i = 0;
	while(new.position < new.length)
	{
		handle_packet(&new);
		libgdbc_push_message(instance, &new);
	}
	// TODO check if its possible that gdbserver sends multiple acks in 1 packet
	instance->acks += new.acks;
	return 0;
}
