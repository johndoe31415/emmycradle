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
#include "sincos.h"

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

static enum mode_t run_mode = MODE_OFF;
static int16_t timer_mode = -1;
static bool update_display = false;

static uint8_t stepping;
static struct {
	struct debounce_t middle;
	struct debounce_t left;
	struct debounce_t right;
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

static void second_passed(void) {
	if (timer_mode > 0) {
		update_display = true;
		timer_mode--;

		/* TODO DEBUG */
#if 0
		if (timer_mode > 30) {
			timer_mode -= 30;
		} else {
			timer_mode = 0;
		}
#endif

		if (timer_mode == 0) {
			run_mode = MODE_OFF;
			timer_mode = -1;
		}
	}
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

static void update_status(enum mode_t mode) {
	if (mode == MODE_OFF) {
		hd44780_print_P(12, 0, PSTR("off "));
	} else if (mode == MODE_SINE) {
		hd44780_print_P(12, 0, PSTR("sine"));
	} else if (mode == MODE_FAST) {
		hd44780_print_P(12, 0, PSTR("fast"));
	} else if (mode == MODE_SLOWING) {
		hd44780_print_P(12, 0, PSTR("swng"));
	}
}

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

int main(void) {
	initHAL();
	hd44780_init();
	hd44780_print_P(0, 0, PSTR("Emmy Cradle"));

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

	/* Put OC1B in PWM mode */
	TCCR1A = _BV(COM1B1) | _BV(WGM11);
	TCCR1B = _BV(CS11) | _BV(WGM13) | _BV(WGM12);	/* CTC mode, CK / 8 */

	/* f = 16e6 / 8 / ICR1 */
	/* Absolute maximum frequency for stepper driver: ~15.4 kHz in 16-step
	 * microstepping mode (ICR1 = 130) */
	ICR1 = 150;
	OCR1B = ICR1 / 2;
	while (true);


	ENABLE_SetActive();

	uint16_t speed = 150;
	int8_t accu = 1;
	while (true) {
		OCR1B = speed / 2;
		ICR1 = speed;

		speed += accu;
		if (speed >= 300) {
			accu = -accu;
			speed = 300;
		} else if (speed <= 150) {
			accu = -accu;
			speed = 150;
		}



//		STEP_Toggle();
		_delay_ms(3);
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
