#include "utils.h"


/**
 * Function creates the checksum
 * for the given command
 * - command : is used to calculate the checksum needs to be null terminated
 * @returns : calculated checksum
 */
uint8_t cmd_checksum(const char* command) {
	uint8_t sum = 0;
	while(*command != '\0') {
		sum += *command++;	
	}
	return sum;
}


/**
 * Converts str to uint64_t
 */
uint64_t unpack_uint64(char *buff, int len) {
  int nibble;
  uint64_t retval = 0;
  while (len) {
		nibble = hex2int(*buff++);
		retval |= nibble;
    len--;
    if (len) retval = retval << 4;
  }
  return retval;
}


/**
 * Changed byte order and
 * converts the value into uint64_t
 */
uint64_t unpack_uint64_co(char* buff, int len) {
	uint64_t result = 0;
	int i;
	for (i = len - 2; i >= 0; i-=2) {
		result |= unpack_uint64(&buff[i], 2);
		if (i) result <<= 8;
	}
	return result;
}


/**
 * Converts a given hex character into its int value
 * @returns value of hex or -1 on error
 */
int hex2int(int ch) {
	if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	if (ch >= '0' && ch <= '9') return ch - '0';
	return -1;
}
