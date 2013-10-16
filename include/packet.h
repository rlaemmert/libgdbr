#ifndef PACKET_H
#define PACKET_H

#include "core.h"
#include <stdint.h>
#include <unistd.h>

typedef struct parsing_object_t
{
	char* buffer;
	ssize_t length;
	int start;
	int end;
	int position;
	uint8_t checksum;
	int acks;
} parsing_object_t;


int parse_packet(libgdbc_t* instance);
void handle_data(parsing_object_t* current);
void handle_chk(parsing_object_t* current);
void handle_packet(parsing_object_t* current);
void handle_escape(parsing_object_t* current);
char get_next_token(parsing_object_t* current);

#endif
