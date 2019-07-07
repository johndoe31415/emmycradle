#include <stdint.h>
#ifdef __AVR
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif
#include "sincos.h"

static const uint8_t delay_values[LOOKUP_TABLE_SIZE] PROGMEM = {
	255,
	191,
	153,
	128,
	110,
	96,
	86,
	77,
	70,
	65,
	60,
	56,
	52,
	49,
	46,
	44,
	42,
	40,
	38,
	36,
	35,
	34,
	33,
	31,
	30,
	30,
	29,
	28,
	27,
	27,
	26,
	25,
	25,
	24,
	24,
	23,
	23,
	23,
	22,
	22,
	22,
	21,
	21,
	21,
	21,
	20,
	20,
	20,
	20,
	20,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19,
	19
};

#ifndef TEST_MAIN
uint8_t lookup_value(uint8_t index) {
	return pgm_read_byte(index + delay_values);
}
#else

#include <stdio.h>
#include <stdbool.h>

#define STEP_SetActive()
#define STEP_SetInactive()
#define sleep_units(x)		printf("Sleep %d\n", x)

uint8_t lookup_value(uint8_t index) {
	return delay_values[index];
}

static void steps_sine(bool invert, uint16_t step_count) {
	uint16_t steps = step_count;
	while (steps--) {
		uint8_t index = LOOKUP_TABLE_SIZE * steps / step_count;
		if (invert) {
			index = LOOKUP_TABLE_SIZE - 1 - index;
		}
		uint16_t delay = lookup_value(index);

		STEP_SetActive();
		sleep_units(delay);
		STEP_SetInactive();
		sleep_units(delay);
	}
}

int main() {
	steps_sine(true, 100);
}

#endif
