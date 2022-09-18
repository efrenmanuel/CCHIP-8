#ifndef RAM_LOADED
#define RAM_LOADED
#include "RAM.h"
#include <malloc.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

Byte c8_ram_buffer[RAM_SIZE];

void c8_ram_initialize() {
	memset(c8_ram_buffer, 0, RAM_SIZE);
}


void c8_ram_store_byte(Byte input, unsigned int location) {
	c8_ram_buffer[location] = input;
}

void c8_ram_store(Byte* input, unsigned int size, unsigned int start) {
	memcpy(&c8_ram_buffer[start], input, size);
}

Byte* c8_ram_fetch(unsigned int start) {
	return &c8_ram_buffer[start];
}

void c8_ram_load_fonts() {
	Byte font_array[80] = {
		0b11110000,
		0b10010000,
		0b10010000,
		0b10010000,
		0b11110000,

		0b00100000,
		0b01100000,
		0b00100000,
		0b00100000,
		0b01110000,

		0b11110000,
		0b00010000,
		0b11110000,
		0b10000000,
		0b11110000,

		0b11110000,
		0b00010000,
		0b11110000,
		0b00010000,
		0b11110000,

		0b10010000,
		0b10010000,
		0b11110000,
		0b00010000,
		0b00010000,

		0b11110000,
		0b10000000,
		0b11110000,
		0b00010000,
		0b11110000,

		0b11110000,
		0b10000000,
		0b11110000,
		0b10010000,
		0b11110000,

		0b11110000,
		0b00010000,
		0b00100000,
		0b01000000,
		0b01000000,

		0b11110000,
		0b10010000,
		0b11110000,
		0b10010000,
		0b11110000,

		0b11110000,
		0b10010000,
		0b11110000,
		0b00010000,
		0b11110000,

		0b11110000,
		0b10010000,
		0b11110000,
		0b10010000,
		0b10010000,

		0b11100000,
		0b10010000,
		0b11100000,
		0b10010000,
		0b11100000,

		0b11110000,
		0b10000000,
		0b10000000,
		0b10000000,
		0b11110000,

		0b11100000,
		0b10010000,
		0b10010000,
		0b10010000,
		0b11100000,

		0b11110000,
		0b10000000,
		0b11110000,
		0b10000000,
		0b11110000,


		0b11110000,
		0b10000000,
		0b11110000,
		0b10000000,
		0b10000000
	};

	memcpy(c8_ram_buffer, font_array, sizeof(Byte)*80);
}


void c8_ram_dump() {
	FILE* output_file;
	fopen_s(&output_file, "RAM.bytes", "wb");

	fwrite(c8_ram_buffer, 1, RAM_SIZE, output_file);
	fclose(output_file);
}
#endif