#!/usr/bin/python

# Script that reads a given .dat file containing a register profile
# for gdb (located at gdb/regformats/) and prints
# a neat struct for libgdbr
import sys

if len(sys.argv) < 2:
    print("Filename missing\n")

offset = 0
with open(sys.argv[1]) as fp:
    for line in fp:
        line = line.rstrip()
        elems = line.split(':')
        if elems[0] == 'name':
            print("static registers_t "+elems[1]+"[] = {")
        elif elems[0].isdigit():
            register_size = int(elems[0]) // 8
            print("\t{\""+elems[1]+"\",\t"+str(offset)+",\t"+str(register_size)+"},")
            offset += register_size
    print("\t{\"\",\t0,\t0}")
    print("};")
