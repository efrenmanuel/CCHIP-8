#ifndef ROM_LOADED
	#define ROM_LOADED
	#include "ROM.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <malloc.h>

	Byte RomBuffer[ROM_SIZE];
	unsigned int ROM_USED_SIZE;

	void c8_rom_initialize() {
		memset(RomBuffer, 0, ROM_SIZE);
	}

	unsigned long getFileLength(FILE* file)
	{
		fseek(file, 0, SEEK_END);
		unsigned long sz = ftell(file);

		fseek(file, 0, SEEK_SET);

		return sz;
	}

	void c8_rom_load(char* filename) {
		FILE* inputFile;
		fopen_s(&inputFile, filename, "rb");
		unsigned long fileLenght = getFileLength(inputFile);
		fread(RomBuffer, 1, fileLenght, inputFile);
		fclose(inputFile);
		ROM_USED_SIZE = fileLenght;
	}

	Byte c8_rom_fetch_byte(unsigned int location) {
		return RomBuffer[location];
	}

	void c8_rom_dump() {
		FILE* outputFile;
		fopen_s(&outputFile, "ROM.bytes", "wb");

		fwrite(RomBuffer, 1, ROM_USED_SIZE, outputFile);
		fclose(outputFile);
	}

#endif // !1