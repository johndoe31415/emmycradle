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

#ifndef __HD44780_H__
#define __HD44780_H__

#define CMD_CLR_DISPLAY							0x01
#define CMD_RETURN_HOME							0x02

// id = 1: Increment
//    = 0: Decrement
// s  = 1: Accompanies display shift
#define CMD_ENTRY_MODE_SET(id, s)				(0x04 | (((id) & 1) << 1) | (((s) & 1) << 0))

// d  = 1: Display
// c  = 1: Cursor on
// b  = 1: Blinking
#define CMD_DISPLAY_ON_OFF(d, c, b)				(0x08 | (((d) & 1) << 2) | (((c) & 1) << 1) | (((b) & 1) << 0))

// sc = 1: Display shift
//    = 0: Cursor move
// rl = 1: Shift to the right
//    = 0: Shift to the left
#define CMD_CURSOR_DISPLAY_SHIFT(sc, rl)		(0x10 | (((sc) & 1) << 3) | (((rl) & 1) << 2))

// dl = 1: 8 bit operation
//    = 0: 4 bit operation
// n  = 1: 2 lines
//    = 0: 1 line
// f  = 1: 5x10 dots
//    = 0: 5x8 dots
#define CMD_FUNCTION_SET(dl, n, f)				(0x20 | (((dl) & 1) << 4) | (((n) & 1) << 3) | (((f) & 1) << 2))
#define CMD_SET_CGRAM_ADDR(acg)					(0x40 | (((acg) & 0x3f)))
#define CMD_SET_DDRAM_ADDR(add)					(0x80 | (((add) & 0x7f)))

/*************** AUTO GENERATED SECTION FOLLOWS ***************/
void hd44780_print_P(uint8_t x, uint8_t y, const char *string);
void hd44780_init(void);
/***************  AUTO GENERATED SECTION ENDS   ***************/

#endif
