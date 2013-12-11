/*! \file */
#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>


uint8_t cmd_checksum(const char* command);
uint64_t unpack_uint64(char *buff, int len);
uint64_t unpack_uint64_co(char* buff, int len);
int hex2int(int ch);
int int2hex(int i);
void hexdump(void* ptr, uint64_t len, uint64_t offset);

#endif
