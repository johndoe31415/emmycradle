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

#define TIMER_INCREMENT_SECONDS		(5 * 60)

enum interpolation_mode_t {
	INTERPOLATION_LINEAR,
	INTERPOLATION_SINUSOIDAL,
};

enum active_menu_t {
	ACTIVE_MENU_WAVEFORM,
	ACTIVE_MENU_TIMER,
	ACTIVE_MENU_SPEED,
	ACTIVE_MENU_LENGTH,
	ACTIVE_MENU_LAST,
};

struct motor_actuation_state_t {
	uint8_t lookup_index;
	bool falling;
	uint16_t distance_travelled;
	uint8_t current_speed;
};

struct system_state_t {
	bool active;
	uint8_t interpolation_mode;
	uint16_t timer_seconds;
	uint8_t speed;
	uint8_t length;
	uint8_t menu_active;
};
static uint8_t stepping;
static struct {
	struct debounce_t middle;
	struct debounce_t left;
	struct debounce_t right;
	struct debounce_t up;
	struct debounce_t down;
} button_state;
static uint8_t buttons_pressed;
static struct system_state_t system_state = {
	.speed = 3,
	.length = 3,
	.interpolation_mode = INTERPOLATION_SINUSOIDAL,
};
static struct motor_actuation_state_t motor_actuation_state;

static char displaybuffer[2 * 16];
static const char char_heart[8] PROGMEM = { 0x0, 0xa, 0x1f, 0x1f, 0x1f, 0xe, 0x4, 0x0 };
static const char char_lin_l[8] PROGMEM = { 0x0, 0x8, 0x14, 0x2, 0x1, 0x0, 0x0, 0x0 };
static const char char_lin_r[8] PROGMEM = { 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x14, 0x8 };
static const char char_sine_l[8] PROGMEM = { 0x0, 0xc, 0x12, 0x1, 0x1, 0x0, 0x0, 0x0 };
static const char char_sine_r[8] PROGMEM = { 0x0, 0x0, 0x0, 0x1, 0x1, 0x2, 0x1c, 0x0 };

#define LOOKUP_TABLE_SIZE				200
#define ABSOLUTE_MINIMUM_SPEED_PERIOD	4906
#define ABSOLUTE_MAXIMUM_SPEED_PERIOD	140
static const uint16_t sine_table[LOOKUP_TABLE_SIZE] PROGMEM = {
	 4906,  3686,  2954,  2466,  2118,  1856,  1653,  1491,  1358,  1247,  1153,  1073,  1003,   943,   889,   841,
	  799,   760,   725,   694,   665,   639,   614,   592,   571,   552,   534,   518,   502,   487,   474,   461,
	  449,   437,   426,   416,   406,   397,   388,   380,   372,   364,   357,   350,   344,   337,   331,   326,
	  320,   315,   309,   304,   300,   295,   291,   286,   282,   278,   274,   271,   267,   264,   260,   257,
	  254,   251,   248,   245,   242,   240,   237,   234,   232,   230,   227,   225,   223,   221,   219,   217,
	  215,   213,   211,   209,   207,   205,   204,   202,   200,   199,   197,   196,   194,   193,   192,   190,
	  189,   188,   186,   185,   184,   183,   182,   180,   179,   178,   177,   176,   175,   174,   173,   172,
	  171,   171,   170,   169,   168,   167,   166,   166,   165,   164,   163,   163,   162,   161,   161,   160,
	  159,   159,   158,   157,   157,   156,   156,   155,   155,   154,   154,   153,   153,   152,   152,   151,
	  151,   150,   150,   149,   149,   149,   148,   148,   148,   147,   147,   147,   146,   146,   146,   145,
	  145,   145,   144,   144,   144,   144,   143,   143,   143,   143,   143,   142,   142,   142,   142,   142,
	  142,   141,   141,   141,   141,   141,   141,   141,   141,   141,   140,   140,   140,   140,   140,   140,
	  140,   140,   140,   140,   140,   140,   140,   140,
};

static const uint16_t lin_table[LOOKUP_TABLE_SIZE] PROGMEM = {
	254, 254, 253, 253, 252, 252, 251, 250, 250, 249, 249, 248, 248, 247, 246, 246,
	245, 245, 244, 244, 243, 242, 242, 241, 241, 240, 239, 239, 238, 238, 237, 237,
	236, 235, 235, 234, 234, 233, 233, 232, 231, 231, 230, 230, 229, 229, 228, 227,
	227, 226, 226, 225, 225, 224, 223, 223, 222, 222, 221, 220, 220, 219, 219, 218,
	218, 217, 216, 216, 215, 215, 214, 214, 213, 212, 212, 211, 211, 210, 210, 209,
	208, 208, 207, 207, 206, 206, 205, 204, 204, 203, 203, 202, 202, 201, 200, 200,
	199, 199, 198, 198, 197, 196, 196, 195, 195, 194, 193, 193, 192, 192, 191, 191,
	190, 189, 189, 188, 188, 187, 187, 186, 185, 185, 184, 184, 183, 183, 182, 181,
	181, 180, 180, 179, 179, 178, 177, 177, 176, 176, 175, 174, 174, 173, 173, 172,
	172, 171, 170, 170, 169, 169, 168, 168, 167, 166, 166, 165, 165, 164, 164, 163,
	162, 162, 161, 161, 160, 160, 159, 158, 158, 157, 157, 156, 156, 155, 154, 154,
	153, 153, 152, 152, 151, 150, 150, 149, 149, 148, 147, 147, 146, 146, 145, 145,
	144, 143, 143, 142, 142, 141, 141, 140,
};

static const uint16_t maximum_speed_table[] PROGMEM = {
	[1] = 300,
	[2] = 260,
	[3] = 220,
	[4] = 200,
	[5] = 190,
	[6] = 170,
	[7] = 160,
	[8] = 150,
	[9] = ABSOLUTE_MAXIMUM_SPEED_PERIOD,
};


static void pwm_enable(void) {
	/* Put OC1B in fast PWM mode (mode 14) */
	/* TCCR1A : COM1B1 COM1B0 WGM11 WGM10                   */
	/* TCCR1B : WGM13 WGM12                  CS12 CS11 CS10 */
	TCNT1 = 0;
	ICR1 = 1000;
	OCR1B = 1001;
	TCCR1A = _BV(COM1B1) | _BV(WGM11);
	TCCR1B = _BV(CS11) | _BV(WGM13) | _BV(WGM12);	/* CK / 8 */
}

static void pwm_set_period(uint16_t period) {
	/* f = 16e6 / 8 / ICR1 */
	/* Absolute maximum frequency for stepper driver: ~15.4 kHz in 16-step
	 * microstepping mode (ICR1 = 130) */
	TCNT1 = 0;
	ICR1 = period;
	OCR1B = period / 2;
}

static void pwm_disable(void) {
	TCCR1A = 0;
}


static void refresh_display(void) {
	hd44780_print_charcnt(0, 0, displaybuffer + (16 * 0), 16);
	hd44780_print_charcnt(0, 1, displaybuffer + (16 * 1), 16);
}

static void strcpy_P_noterm(char *dest, const char *string) {
	char character;
	while ((character = pgm_read_byte(string++)) != 0) {
		*dest = character;
		dest++;
	}
}

static void strcpy_noterm(char *dest, const char *string) {
	char character;
	while ((character = *string++) != 0) {
		*dest = character;
		dest++;
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

static void state_to_display(void) {
	memset(displaybuffer, ' ', 32);
	if (system_state.active) {
		strcpy_P_noterm(displaybuffer + 0, PSTR("On"));
	} else {
		strcpy_P_noterm(displaybuffer + 0, PSTR("Off"));
	}

	if (system_state.interpolation_mode == INTERPOLATION_LINEAR) {
		strcpy_P_noterm(displaybuffer + 5, PSTR("\x01\x02\x01"));
	} else {
		strcpy_P_noterm(displaybuffer + 5, PSTR("\x03\x04\x03"));
	}

	strcpy_P_noterm(displaybuffer + 10, PSTR("Tmr"));
	if (system_state.timer_seconds > 0) {
		char minutes[8];
		itoa(minutes, (system_state.timer_seconds + 59) / 60);
		strcpy_noterm(displaybuffer + 14, minutes);
	} else {
		strcpy_noterm(displaybuffer + 14, "-");
	}

	strcpy_P_noterm(displaybuffer + 16 + 1, PSTR("Spd"));
	displaybuffer[16 + 5] = system_state.speed + '0';

	strcpy_P_noterm(displaybuffer + 16 + 10, PSTR("Len"));
	displaybuffer[16 + 14] = system_state.length + '0';

	if (system_state.menu_active == ACTIVE_MENU_WAVEFORM) {
		displaybuffer[4] = '~';
	} else if (system_state.menu_active == ACTIVE_MENU_TIMER) {
		displaybuffer[9] = '~';
	} else if (system_state.menu_active == ACTIVE_MENU_SPEED) {
		displaybuffer[16 + 0] = '~';
	} else if (system_state.menu_active == ACTIVE_MENU_LENGTH) {
		displaybuffer[16 + 9] = '~';
	}

#if 0
	{
		char temp[8];
		itoa(temp, motor_actuation_state.distance_travelled);
		memset(displaybuffer, ' ', 32);
		strcpy_noterm(displaybuffer + 0, temp);
	}
#endif
}

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

static void check_buttons(void) {
	if (buttons_pressed & BUTTON_MIDDLE) {
		system_state.active = !system_state.active;
		if (system_state.active) {
			pwm_enable();
			ENABLE_SetActive();
		}
	}
	if (buttons_pressed & BUTTON_LEFT) {
		system_state.menu_active -= 1;
		if (system_state.menu_active == 255) {
			system_state.menu_active = ACTIVE_MENU_LAST - 1;
		}
	}
	if (buttons_pressed & BUTTON_RIGHT) {
		system_state.menu_active += 1;
		if (system_state.menu_active == ACTIVE_MENU_LAST) {
			system_state.menu_active = 0;
		}
	}

	if (buttons_pressed & BUTTON_UP) {
		if (system_state.menu_active == ACTIVE_MENU_WAVEFORM) {
			system_state.interpolation_mode = (system_state.interpolation_mode == INTERPOLATION_LINEAR) ? INTERPOLATION_SINUSOIDAL : INTERPOLATION_LINEAR;
		}
		if (system_state.menu_active == ACTIVE_MENU_TIMER) {
			if (system_state.timer_seconds < 3600 - TIMER_INCREMENT_SECONDS) {
				system_state.timer_seconds += TIMER_INCREMENT_SECONDS;
			}
		}
		if (system_state.menu_active == ACTIVE_MENU_SPEED) {
			if (system_state.speed < 9) {
				system_state.speed += 1;
			}
		}
		if (system_state.menu_active == ACTIVE_MENU_LENGTH) {
			if (system_state.length < 9) {
				system_state.length += 1;
			}
		}
	}

	if (buttons_pressed & BUTTON_DOWN) {
		if (system_state.menu_active == ACTIVE_MENU_WAVEFORM) {
			system_state.interpolation_mode = (system_state.interpolation_mode == INTERPOLATION_LINEAR) ? INTERPOLATION_SINUSOIDAL : INTERPOLATION_LINEAR;
		}
		if (system_state.menu_active == ACTIVE_MENU_TIMER) {
			if (system_state.timer_seconds >= TIMER_INCREMENT_SECONDS) {
				system_state.timer_seconds -= TIMER_INCREMENT_SECONDS;
			} else {
				system_state.timer_seconds = 0;
			}
		}
		if (system_state.menu_active == ACTIVE_MENU_SPEED) {
			if (system_state.speed > 1) {
				system_state.speed -= 1;
			}
		}
		if (system_state.menu_active == ACTIVE_MENU_LENGTH) {
			if (system_state.length > 1) {
				system_state.length -= 1;
			}
		}
	}

	buttons_pressed = 0;
}

static void second_passed(void) {
	static uint8_t last_reduction_ago_secs = 0;
	if (last_reduction_ago_secs < 0xff) {
		last_reduction_ago_secs++;
	}

	if (system_state.timer_seconds) {
		system_state.timer_seconds--;
		if (system_state.timer_seconds == 0) {
			/* Shutoff. */
			system_state.active = false;
		} else {
			if (last_reduction_ago_secs > 30) {
				/* Reduce speed only at max every 30 seconds */
				bool want_reduce_speed = false;
				for (uint8_t speed = 1; speed <= 9; speed++) {
					want_reduce_speed |= ((system_state.timer_seconds <= speed * 60) && (system_state.speed > speed));
				}
				if (want_reduce_speed) {
					last_reduction_ago_secs = 0;
					system_state.speed--;
				}
			}
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
	if (debounce(&button_state.up, SWITCH_UP_IsActive()) == ACTION_PRESSED) {
		buttons_pressed |= BUTTON_UP;
	}
	if (debounce(&button_state.down, SWITCH_DOWN_IsActive()) == ACTION_PRESSED) {
		buttons_pressed |= BUTTON_DOWN;
	}

	static uint8_t second_tick = 250;
	second_tick--;
	if (second_tick == 0) {
		second_tick = 250;
		second_passed();
	}
}

static void motor_switch_direction(void) {
	motor_actuation_state.distance_travelled = 0;
	DIRECTION_Toggle();
}

/* Called approximately every millisecond */
static void actuate_motor(void) {
	/* Distance travelled is the integral of speed over time */
	motor_actuation_state.distance_travelled += motor_actuation_state.current_speed;

	if (!motor_actuation_state.falling) {
		/* Rising edge, increase table index */
		if (motor_actuation_state.lookup_index < LOOKUP_TABLE_SIZE - 1) {
			motor_actuation_state.lookup_index++;
		}
	} else {
		/* Falling edge, decrease table index */
		if (motor_actuation_state.lookup_index) {
			motor_actuation_state.lookup_index--;
		} else {
			/* We're at speed zero, switch direction */
			motor_switch_direction();
			motor_actuation_state.falling = false;
		}
	}

	/* Choose the proper lookup table */
	const uint16_t *table = (system_state.interpolation_mode == INTERPOLATION_SINUSOIDAL) ? sine_table : lin_table;

	/* Read the period word from the table */
	uint16_t period = pgm_read_word(table + motor_actuation_state.lookup_index);

	/* Limit the value of the period word depending on the maximum speed */
	if ((system_state.speed >= 1) && (system_state.speed <= 9)) {
		uint16_t maximum_speed_period = pgm_read_word(maximum_speed_table + system_state.speed);
		if (period < maximum_speed_period) {
			period = maximum_speed_period;
		}
	}

	/* Never allow faster than this, however */
	if (period < ABSOLUTE_MAXIMUM_SPEED_PERIOD) {
		period = ABSOLUTE_MAXIMUM_SPEED_PERIOD;
	}
	/* Or slower than this */
	if (period > ABSOLUTE_MINIMUM_SPEED_PERIOD) {
		period = ABSOLUTE_MINIMUM_SPEED_PERIOD;
	}

	/* From that period, calculate the new motor speed */
	motor_actuation_state.current_speed = ABSOLUTE_MINIMUM_SPEED_PERIOD / period;

	/* Once we've fulfilled the required distance, we start initiating the backoff */
	uint16_t required_distance = 2000 + (system_state.length * 1000);

	if (motor_actuation_state.distance_travelled > required_distance) {
		if (!motor_actuation_state.falling) {
			/* Slow down motor, but still keep direction */
			/* We're currently full speed ahead, cannot reverse now */
			motor_actuation_state.falling = true;
		}
	}

	/* Finally set the appropriate PWM */
	pwm_set_period(period);
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

	memcpy_P(displaybuffer, PSTR("    From Dad    "
				                 "We love you Emmy"), 32);
	refresh_display();
	_delay_ms(1000);

	memset(displaybuffer, ' ', 32);
	strcpy_P_noterm(displaybuffer, PSTR("Git commit:"));
	strcpy_P_noterm(displaybuffer + 16, PSTR(BUILD_REVISION));
	refresh_display();
	_delay_ms(1000);

	memset(displaybuffer, ' ', 32);
	refresh_display();

	/* Setup timer for polling buttons */
	TCCR0 = _BV(CS02) | _BV(CS01);	// CK / 256, i.e. OVF 4.096ms @ 16 MHz
	TIMSK |= _BV(TOIE0);

	/* Enable IRQs */
	sei();

	/* Initialize A4988 */
	DIRECTION_SetInactive();
	RESET_SetInactive();
	SLEEP_SetInactive();
	ENABLE_SetInactive();
	set_stepping(16);

	while (true) {
		if (!system_state.active) {
			ENABLE_SetInactive();
			STEP_SetInactive();
			pwm_disable();
			memset(&motor_actuation_state, 0, sizeof(struct motor_actuation_state_t));
		} else {
			actuate_motor();
		}

		check_buttons();
		state_to_display();
		refresh_display();
	}
	return 0;
}
