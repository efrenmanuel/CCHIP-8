#pragma once
#include "Global.h"
unsigned short c8_kb_pressed;
extern Byte c8_kb_waiting;

void c8_kb_press_key(Byte key);
void c8_kb_release_key(Byte key);
Byte c8_kb_is_pressed(Byte key);
Byte c8_kb_wait();
Byte c8_kb_pop_wait_key();