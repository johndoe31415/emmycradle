#!/usr/bin/python3
#
#

import math

table_size = 64

print("#include <stdint.h>")
print("#include <avr/pgmspace.h>")
print("#include \"sincos.h\"")
print()
print("static const uint8_t delay_values[LOOKUP_TABLE_SIZE] PROGMEM = {")
scale = 255 / 3466
for i in range(3, table_size):
	x = i / table_size * math.pi / 2
	y = round(255 / math.sin(x) * scale)
	print("	%d," % (y))
print("};")
