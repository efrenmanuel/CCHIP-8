#ifndef KB_LOADED
	#define KB_LOADED
	#include "Keyboard.h"
	unsigned short c8_kb_pressed;
	Byte c8_kb_waiting;
	Byte c8_kb_wait_pressed;
	Byte c8_kb_last_key;

	/*
	1	2	3	C
	4	5	6	D
	7	8	9	E
	A	0	B	F
	*/

	void c8_kb_initialize() {
		c8_kb_waiting = 0;
	}

	void c8_kb_press_key(Byte key) {
		if (c8_kb_waiting == 1){
			c8_kb_last_key = key;
			c8_kb_wait_pressed = 1;
		}
		c8_kb_pressed |= (1 << key);
	}
	void c8_kb_release_key(Byte key) {
		c8_kb_pressed &= ~(1 << key);
	}
		
	Byte c8_kb_is_pressed(Byte key) {
		return (c8_kb_pressed & (1 << key)) > 0 ? 1 : 0;
	}

	Byte c8_kb_wait() {
		c8_kb_waiting = 1;
		if (c8_kb_wait_pressed == 1) {
			return 1;
		}
		else {
			return 0;
		}
		
	}

	Byte c8_kb_pop_wait_key() {
		c8_kb_waiting = 0;
		c8_kb_wait_pressed = 0;
		return c8_kb_last_key;
	}

#endif