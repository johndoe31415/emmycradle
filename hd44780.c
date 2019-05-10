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

#include <avr/pgmspace.h>
#include <util/delay.h>
#include "hal.h"
#include "hd44780.h"

static void send_4bit(uint8_t data) {
	DISPLAY_E_SetActive();
	DISPLAY_D7_SetConditional(data & (1 << 3));
	DISPLAY_D6_SetConditional(data & (1 << 2));
	DISPLAY_D5_SetConditional(data & (1 << 1));
	DISPLAY_D4_SetConditional(data & (1 << 0));
	DISPLAY_E_SetInactive();
	_delay_us(1);
}

static void send_8bit(uint8_t data) {
	DISPLAY_READ_SetInactive();
	DISPLAY_D7_ModeOutput();
	DISPLAY_D6_ModeOutput();
	DISPLAY_D5_ModeOutput();
	DISPLAY_D4_ModeOutput();
	send_4bit(data >> 4);
	send_4bit(data);
}

static void send_cmd(uint8_t cmd) {
	DISPLAY_RS_SetInactive();
	send_8bit(cmd);
}

static void send_data(uint8_t cmd) {
	DISPLAY_RS_SetActive();
	send_8bit(cmd);
}

static void delay_millis(uint8_t millis) {
	while (millis--) {
		_delay_ms(1);
	}
}

void hd44780_print_cursor(const char *string) {
	uint8_t character;
	while ((character = *string++) != 0) {
		send_data(character);
		delay_millis(1);
	}
}

void hd44780_print_P_cursor(const char *string) {
	uint8_t character;
	while ((character = pgm_read_byte(string++)) != 0) {
		send_data(character);
		delay_millis(1);
	}
}

void hd44780_print_P(uint8_t x, uint8_t y, const char *string) {
	uint8_t rampos = ((y == 0) ? 0 : 0x40) + x;
	send_cmd(CMD_SET_DDRAM_ADDR(rampos));
	delay_millis(1);
	hd44780_print_P_cursor(string);
}

void hd44780_init(void) {
	DISPLAY_READ_SetInactive();
	DISPLAY_RS_SetInactive();
	DISPLAY_E_SetInactive();
	DISPLAY_D7_ModeOutput();
	DISPLAY_D6_ModeOutput();
	DISPLAY_D5_ModeOutput();
	DISPLAY_D4_ModeOutput();

	delay_millis(40);
	send_4bit(0x03);

	delay_millis(5);
	send_4bit(0x03);

	delay_millis(5);
	send_4bit(0x03);

	delay_millis(1);
	send_4bit(0x02);
	delay_millis(1);

	send_cmd(CMD_FUNCTION_SET(0, 1, 0));
	delay_millis(4);

	send_cmd(CMD_DISPLAY_ON_OFF(1, 0, 0));
	delay_millis(4);

	send_cmd(CMD_CLR_DISPLAY);
	delay_millis(4);
}
