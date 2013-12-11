#include "libgdbr.h"
#include "core.h"

#include <stdio.h>


int libgdbr_send_cmd(char* command) {
	int acks = instance.acks;
	if (send_command(&instance, command) == 0) {
		if (acks < instance.acks) {
			/* the last send_command was a success so handling the answer now */
			char* answer = pop_message(&instance);
			instance.acks--;
//			printf("Msg: %s\n", answer);
			free(answer);
		}
	} else {
		return -1;
	}
}
