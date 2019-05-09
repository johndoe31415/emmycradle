.PHONY: all clean program reset halgen

DEVICE := atmega128
DUDE_DEV := m128
CLOCK := 16000000
OBJS := emmycradle.o debounce.o

CC := avr-gcc
CFLAGS := -std=c11 -g3 -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
CFLAGS += -Wall -Wmissing-prototypes -Wstrict-prototypes -Werror=implicit-function-declaration -Werror=format -Wshadow

all: emmycradle emmycradle.bin

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

reset:
	avrdude -c avrispmkII -p $(DUDE_DEV) -B 10

program: emmycradle.hex
	avrdude -c avrispmkII -p $(DUDE_DEV) -B 4 -U flash:w:emmycradle.hex:i

clean:
	rm -f emmycradle.hex emmycradle.bin emmycradle $(OBJS)

emmycradle: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

emmycradle.hex: emmycradle
	avr-objcopy -j .text -j .data -O ihex emmycradle $@

emmycradle.bin: emmycradle
	avr-objcopy -j .text -j .data -O binary emmycradle $@

halgen: hal.xml
	../HALGen/halgen -a avr -o hal.h hal.xml
