#ifndef GPU_LOADED
	#define GPU_LOADED
	#include <string.h>
	#include <stdio.h>

	#include "GPU.h"
	#include "Global.h"
	#include "CNFG.h"
	#include "CPU.h"

	unsigned long long screen_buffer[SCREEN_BUFFER_ROWS];

	void c8_gpu_flip_pixel(short x, short y) {
		unsigned long long one = 1;
		screen_buffer[y] ^= one << x;
		if (((screen_buffer[y] >> x) & 1) == 0) {
			r_VF = 1;
		}
	}

	void c8_gpu_display_sprite(short start_x, short start_y, Byte* sprite, int rows) {
		for (int y = 0; y < rows; y++) {
			unsigned long long row = screen_buffer[y];
			Byte sprite_row = sprite[y];
			for (int x = 0; x < 8; x++) {
				if(((sprite_row >> x) & 1) > 0 ){
					c8_gpu_flip_pixel((7-x + start_x) % 64, (y + start_y % 32));
				}
			}
		}
		int x = 0;
	}

	void c8_gpu_refresh_display() {

		CNFGClearFrame();

		for (int y = 0; y < SCREEN_BUFFER_ROWS; y++) {
			unsigned long long row = screen_buffer[y];
			for (int x = 0; x < SCREEN_BUFFER_COLS; x++) {
				Byte value = GET_BIT(row, x);
				if (value == 0) { CNFGColor(0x00000000); }
				else CNFGColor(0x00FFFFFF);
				CNFGTackRectangle(x * PIXEL_SCALE, y * PIXEL_SCALE, (x + 1) * PIXEL_SCALE, (y + 1) * PIXEL_SCALE);
			}
		}

		CNFGSwapBuffers();
	}


	void c8_gpu_initialize() {
		memset(screen_buffer, 0, 32 * sizeof(unsigned long long));
	}

	void c8_gpu_clear() {
		memset(screen_buffer, 0, 32 * sizeof(unsigned long long));
	}

	void c8_gpu_cls() {

	}

	void c8_gpu_dump_buffer() {
		FILE* outputFile;
		fopen_s(&outputFile, "SCREEN.bytes", "wb");

		fwrite(screen_buffer, 1, sizeof(unsigned long long) * SCREEN_BUFFER_ROWS, outputFile);
		fclose(outputFile);
	}

#endif