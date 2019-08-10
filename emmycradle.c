/*
	emmycradle - AVR-based stepper motor driver for a baby cradle
	Copyright (C) 2019-2019 Johannes Bauer

	This file is part of emmycradle.

	emmycradle is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; this program is ONLY licensed under
	version 3 of the License, later versions are explicitly excluded.

	emmycradle is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with emmycradle; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Johannes Bauer <JohannesBauer@gmx.de>
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "hal.h"
#include "debounce.h"
#include "hd44780.h"

#define BUTTON_MIDDLE		(1 << 0)
#define BUTTON_LEFT			(1 << 1)
#define BUTTON_RIGHT		(1 << 2)
#define BUTTON_UP			(1 << 3)
#define BUTTON_DOWN			(1 << 4)

enum mode_t {
	MODE_OFF,
	MODE_SLOWING,
	MODE_FAST,
	MODE_SINE,
	MODE_INVALID
};

enum interpolation_mode_t {
	INTERPOLATION_LINEAR,
	INTERPOLATION_SINUSOIDAL,
};

struct system_state_t {
	bool active;
	uint8_t interpolation_mode;
	uint16_t timer_seconds;
	uint8_t speed;
	uint8_t length;
};
static uint8_t stepping;
static struct {
	struct debounce_t middle;
	struct debounce_t left;
	struct debounce_t right;
} button_state;
static uint8_t buttons_pressed;

static const uint8_t sine_table[] PROGMEM = {
	140, 141, 142, 143, 144, 145, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154,
	154, 155, 156, 157, 158, 159, 160, 161, 162, 162, 163, 164, 165, 166, 167, 168,
	169, 169, 170, 171, 172, 173, 174, 175, 176, 176, 177, 178, 179, 180, 181, 181,
	182, 183, 184, 185, 186, 186, 187, 188, 189, 190, 191, 191, 192, 193, 194, 195,
	195, 196, 197, 198, 199, 199, 200, 201, 202, 202, 203, 204, 205, 205, 206, 207,
	208, 208, 209, 210, 210, 211, 212, 213, 213, 214, 215, 215, 216, 217, 217, 218,
	219, 219, 220, 221, 221, 222, 223, 223, 224, 224, 225, 226, 226, 227, 227, 228,
	229, 229, 230, 230, 231, 231, 232, 233, 233, 234, 234, 235, 235, 236, 236, 237,
	237, 238, 238, 239, 239, 239, 240, 240, 241, 241, 242, 242, 242, 243, 243, 244,
	244, 244, 245, 245, 246, 246, 246, 247, 247, 247, 248, 248, 248, 249, 249, 249,
	249, 250, 250, 250, 250, 251, 251, 251, 251, 252, 252, 252, 252, 252, 253, 253,
	253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
};
#define SINETABLE_SIZE		(sizeof(sine_table) / sizeof(uint8_t))

static void set_stepping(uint8_t step) {
	MS1_SetInactive();
	MS2_SetInactive();
	MS3_SetInactive();
	if (step == 1) {
		stepping = 0;
	} else if (step == 2) {
		MS1_SetActive();					// half step
		stepping = 1;
	} else if (step == 4) {
		MS2_SetActive();					// 4th step
		stepping = 2;
	} else if (step == 8) {
		MS1_SetActive(); MS2_SetActive();	// 8th step
		stepping = 3;
	} else if (step == 16) {
		MS1_SetActive(); MS2_SetActive(); MS3_SetActive();	// 16th step
		stepping = 4;
	} else {
		stepping = 0;
	}
}
#if 0
static void sleep_units(uint16_t units) {
	while (units--) {
		__asm__ __volatile__("");
	}
}

static void steps(uint16_t stepcount, uint16_t start_delay, uint16_t end_delay) {
	/* Number of steps needs to be larger when we microstep */
	uint16_t steps = stepcount << stepping;
	uint16_t midpoint = steps / 2;
	uint16_t delay = start_delay;

	uint16_t delay_diff = start_delay - end_delay;
	uint16_t full_slope = delay_diff / midpoint;
	uint16_t residual_slope = delay_diff % midpoint;
	uint16_t accumulator = 0;

	while (steps--) {
		STEP_SetActive();
		sleep_units(delay);
		STEP_SetInactive();
		sleep_units(delay);

		int8_t scalar;
		if (steps > midpoint) {
			/* Still accelerating, decrease delay */
			scalar = -1;
		} else {
			/* Now decelerating, increase delay */
			scalar = 1;
		}

		delay += scalar * full_slope;
		accumulator += residual_slope;
		if (accumulator >= midpoint) {
			delay += scalar;
			accumulator -= midpoint;
		}
	}
}
#endif

static void second_passed(void) {
}

ISR(TIMER0_OVF_vect) {
	if (debounce(&button_state.middle, SWITCH_MIDDLE_IsActive()) == ACTION_PRESSED) {
		buttons_pressed |= BUTTON_MIDDLE;
	}
	if (debounce(&button_state.left, SWITCH_LEFT_IsActive()) == ACTION_PRESSED) {
		buttons_pressed |= BUTTON_LEFT;
	}
	if (debounce(&button_state.right, SWITCH_RIGHT_IsActive()) == ACTION_PRESSED) {
		buttons_pressed |= BUTTON_RIGHT;
	}

	static uint8_t second_tick = 250;
	second_tick--;
	if (second_tick == 0) {
		second_tick = 250;
		second_passed();
	}
}
#if 0
static void itoa(char *string, int16_t value) {
	bool first = true;

	char *pos = string;
	while ((value > 0) || first) {
		uint8_t digit = value % 10;
		*pos = digit + '0';
		pos++;
		value /= 10;
		first = false;
	}
	*pos = 0;

	strrev(string);
}
#endif

#if 0
static void update_timer_display(int16_t time_secs) {
	if (time_secs <= 0) {
		hd44780_print_P(0, 1, PSTR("Timer: off      "));
	} else {
		hd44780_print_P(0, 1, PSTR("Timer: "));

		char string[16];
		int16_t minutes = (time_secs + 30) / 60;
		itoa(string, minutes);
		hd44780_print_cursor(string);
		hd44780_print_P_cursor(PSTR(" min   "));
	}
}

static void steps_sine(bool invert, uint16_t step_count, uint8_t repetitions) {
	uint16_t steps = step_count;
	while (steps--) {
		uint8_t index = (uint32_t)LOOKUP_TABLE_SIZE * steps / step_count;
		if (invert) {
			index = LOOKUP_TABLE_SIZE - 1 - index;
		}
		uint16_t delay = 8 * lookup_value(index);

		for (uint8_t i = 0; i < repetitions; i++) {
			STEP_SetActive();
			sleep_units(delay);
			STEP_SetInactive();
			sleep_units(delay);
		}
	}
}
#endif


static char displaybuffer[2 * 16];

static const char char_heart[8] PROGMEM = { 0x0, 0xa, 0x1f, 0x1f, 0x1f, 0xe, 0x4, 0x0 };
static const char char_lin_l[8] PROGMEM = { 0x0, 0x8, 0x14, 0x2, 0x1, 0x0, 0x0, 0x0 };
static const char char_lin_r[8] PROGMEM = { 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x14, 0x8 };
static const char char_sine_l[8] PROGMEM = { 0x0, 0xc, 0x12, 0x1, 0x1, 0x0, 0x0, 0x0 };
static const char char_sine_r[8] PROGMEM = { 0x0, 0x0, 0x0, 0x1, 0x1, 0x2, 0x1c, 0x0 };

static void refresh_display(void) {
	hd44780_print_charcnt(0, 0, displaybuffer + (16 * 0), 16);
	hd44780_print_charcnt(0, 1, displaybuffer + (16 * 1), 16);
}

int main(void) {
	initHAL();
	hd44780_init();

	hd44780_define_custom_char_P(0, char_heart);
	hd44780_define_custom_char_P(1, char_lin_l);
	hd44780_define_custom_char_P(2, char_lin_r);
	hd44780_define_custom_char_P(3, char_sine_l);
	hd44780_define_custom_char_P(4, char_sine_r);

	memcpy_P(displaybuffer, PSTR("  \x00\x00\x00 Emmy \x00\x00\x00  "
				                 " \x00\x00\x00 Cradle \x00\x00\x00 "), 32);
	refresh_display();
	_delay_ms(1000);
	memset(displaybuffer, ' ', 32);
	refresh_display();

	/* Setup timer for polling buttons */
	//TCCR0 = _BV(CS02);	// CK / 64
	TCCR0 = _BV(CS02) | _BV(CS01);	// CK / 256		??? why does this work? CPU runs 8 times faster than expected?
	TIMSK |= _BV(TOIE0);

	/* Enable IRQs */
	sei();

	/* Initialize A4988 */
	DIRECTION_SetInactive();
	RESET_SetInactive();
	SLEEP_SetInactive();
	ENABLE_SetInactive();
	set_stepping(16);

	/* Put OC1B in fast PWM mode (Mode14) */
	/* TCCR1A : COM1B1 COM1B0 WGM11 WGM10 */
	/* TCCR1B : WGM13 WGM12               CS12 CS11 CS10 */
	TCCR1A = _BV(COM1B1) | _BV(WGM11);
	TCCR1B = _BV(CS11) | _BV(WGM13) | _BV(WGM12);	/* CTC mode, CK / 8 */

	/* f = 16e6 / 8 / ICR1 */
	/* Absolute maximum frequency for stepper driver: ~15.4 kHz in 16-step
	 * microstepping mode (ICR1 = 130) */
	ENABLE_SetActive();
	ICR1 = 500;
	OCR1B = ICR1 / 2;

	int16_t index = 0;
	int16_t count = 1;
	while (true) {
		uint16_t period = pgm_read_byte(sine_table + index);
		TCNT1 = 0;
		ICR1 = period;
		OCR1B = period / 2;
		index = index + count;
		if (index < 0) {
			index = 1;
			count = -count;
		} else if (index >= SINETABLE_SIZE) {
			index = SINETABLE_SIZE - 2;
			count = -count;
		}

		_delay_ms(1);
		displaybuffer[0]++;
		refresh_display();
	}

#if 0
	update_status(run_mode);
	update_timer_display(timer_mode);
	while (true) {
		if (run_mode == MODE_SINE) {
			ENABLE_SetActive();
			/* Pull back */
			DIRECTION_SetInactive();
			steps_sine(false, 270, 30);

			/* Then release */
			DIRECTION_SetActive();
			steps_sine(true, 270, 30);
		} else if (run_mode == MODE_FAST) {
			ENABLE_SetActive();
			/* Pull back */
			DIRECTION_SetInactive();
			steps(700, 200, 100);

			/* Then release */
			DIRECTION_SetActive();
			steps(700, 200, 100);
		} else if (run_mode == MODE_SLOWING) {
			if (timer_mode > 0) {
				uint16_t speed = (timer_mode > 800) ? 0 : (800 - timer_mode) / 4;
				ENABLE_SetActive();

				/* Pull back */
				DIRECTION_SetInactive();
				steps(500, 200 + speed, 100 + speed);

				/* Then release */
				DIRECTION_SetActive();
				steps(500, 200 + speed, 100 + speed);
			} else {
				ENABLE_SetInactive();
			}
		} else if (run_mode == MODE_OFF) {
			ENABLE_SetInactive();
		}

		if (buttons_pressed & BUTTON_MIDDLE) {
			run_mode++;
			if (run_mode == MODE_INVALID) {
				run_mode = MODE_OFF;
			}
			update_display = true;
		}
		if (buttons_pressed & BUTTON_RIGHT) {
			if (timer_mode <= 0) {
				timer_mode = 0;
			}
			timer_mode += 10 * 60;
			update_display = true;
		}
		buttons_pressed = 0;
		if (update_display) {
			update_display = false;
			update_status(run_mode);
			update_timer_display(timer_mode);
		}
	}
#endif
	return 0;
}
