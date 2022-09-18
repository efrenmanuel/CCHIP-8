#pragma once
#define RAM_SIZE 4096
#include "Global.h"

void c8_ram_initialize();

void c8_ram_store_byte(Byte input, unsigned int location);

void c8_ram_store(Byte* input, unsigned int size, unsigned int start);

Byte* c8_ram_fetch(unsigned int location);

void c8_ram_dump();

void c8_ram_load_fonts();