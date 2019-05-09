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
#include <util/delay.h>
#include "hal.h"

static uint8_t stepping;

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

static void sleep_millis(uint16_t millis) {
	while (millis--) {
		_delay_ms(1);
	}
}

static void sleep_micros(uint16_t micros) {
	while (micros--) {
		_delay_us(1);
	}
}

static void steps(uint16_t stepcount) {
	stepcount <<= stepping;
	uint16_t delay = 500 >> stepping;
	while (stepcount--) {
		STEP_SetActive();
		sleep_micros(delay);
		STEP_SetInactive();
		sleep_micros(delay);
	}
}

int main(void) {
	initHAL();

	/* Initialize A4988 */
	DIRECTION_SetInactive();
	RESET_SetInactive();
	SLEEP_SetInactive();
	set_stepping(16);

	while (true) {
		/* Pull back */
		ENABLE_SetActive();
		steps(400);

		/* Then release */
		ENABLE_SetInactive();
		sleep_millis(200);
	}
	return 0;
}
