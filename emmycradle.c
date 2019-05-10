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
	MODE_SLOW,
	MODE_FAST,
	MODE_INVALID
};

static uint8_t stepping;
static struct {
	struct debounce_t middle;
} button_state;
static uint8_t buttons_pressed;

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



ISR(TIMER0_OVF_vect) {
	if (debounce(&button_state.middle, SWITCH_MIDDLE_IsActive()) == ACTION_PRESSED) {
		buttons_pressed |= BUTTON_MIDDLE;
	}
}

static void update_status(enum mode_t mode) {
	if (mode == MODE_OFF) {
		hd44780_print_P(12, 0, PSTR("off "));
	} else if (mode == MODE_SLOW) {
		hd44780_print_P(12, 0, PSTR("slow"));
	} else if (mode == MODE_FAST) {
		hd44780_print_P(12, 0, PSTR("fast"));
	}
}

int main(void) {
	initHAL();
	hd44780_init();
	hd44780_print_P(0, 0, PSTR("Emmy Cradle"));

	/* Setup timer for polling buttons */
	TCCR0 = _BV(CS02);	// CK / 64
	TIMSK |= _BV(TOIE0);

	/* Enable IRQs */
	sei();

	/* Initialize A4988 */
	DIRECTION_SetInactive();
	RESET_SetInactive();
	SLEEP_SetInactive();
	ENABLE_SetInactive();
	set_stepping(16);

	enum mode_t mode = MODE_OFF;
	update_status(mode);
	while (true) {
		if (mode == MODE_SLOW) {
			ENABLE_SetActive();
			/* Pull back */
			DIRECTION_SetInactive();
			steps(500, 300, 200);

			/* Then release */
			DIRECTION_SetActive();
			steps(500, 300, 200);
		} else if (mode == MODE_FAST) {
			ENABLE_SetActive();
			/* Pull back */
			DIRECTION_SetInactive();
			steps(700, 200, 100);

			/* Then release */
			DIRECTION_SetActive();
			steps(700, 200, 100);
		} else if (mode == MODE_OFF) {
			ENABLE_SetInactive();
		}

		if (buttons_pressed & BUTTON_MIDDLE) {
			mode++;
			if (mode == MODE_INVALID) {
				mode = MODE_OFF;
			}
			update_status(mode);
		}
		buttons_pressed = 0;
	}
	return 0;
}
