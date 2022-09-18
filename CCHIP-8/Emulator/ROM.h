#pragma once
#define ROM_SIZE 4096
#include "Global.h"

extern unsigned int ROM_USED_SIZE;

void c8_rom_initialize();

void c8_rom_load(char* filename);

Byte c8_rom_fetch_byte(unsigned int location);

void c8_rom_dump();
