//Copyright (c) 2011 <>< Charles Lohr - Under the MIT/x11 or NewBSD License you choose.

//GRAPHICS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "os_generic.h"

#include <direct.h>
#define GetCurrentDir _getcwd

#define CNFG_IMPLEMENTATION
#define CNFGOGL

#include "CNFG.h"

//EMULATOR

#include "Global.h"
#include "RAM.c"
#include "ROM.c"
#include "CPU.c"
#include "GPU.c"
#include "keyboard.c"
#if WINDOWS == 1
	#include "Buzzer_win.h"
#endif

//

#include <stdio.h>
#include <limits.h>

unsigned ticks = 0;
unsigned long iframeno = 0;

double tick_time = 1. / PROCESSOR_HZ;

void HandleKey(int keycode, int bDown)
{
	int key = -1;
	switch (keycode)
	{
	case 27:
		if (DEBUGGING){
			c8_gpu_dump_buffer();
			c8_ram_dump();
		}
		exit(0);
		break;
	case 88:
		key = 0;
		break;
	case 49:
		key = 1;
		break;
	case 50:
		key = 2;
		break;
	case 51:
		key = 3;
		break;
	case 81:
		key = 4;
		break;
	case 87:
		key = 5;
		break;
	case 69:
		key = 6;
		break;
	case 65:
		key = 7;
		break;
	case 83:
		key = 8;
		break;
	case 68:
		key = 9;
		break;
	case 90:
		key = 0xA;
		break;
	case 67:
		key = 0xB;
		break;
	case 52:
		key = 0xC;
		break;
	case 82:
		key = 0xD;
		break;
	case 70:
		key = 0xE;
		break;
	case 86:
		key = 0xF;
		break;
	default:
		printf("key: %d, bdown %d\n", keycode, bDown);
		break;
	}
	if (key >= 0) {
		if (bDown == 1){
			c8_kb_press_key(key);
		}
		else {
			c8_kb_release_key(key);
		}
	}
	
	

	/*
	 1 2 3 C                                   1 2 3 4
	 4 5 6 D    This is emulated with these    Q W E R
	 7 8 9 E    keyboard keys -->              A S D F
	 A 0 B F                                   Z X C V
	*/

}

void HandleButton(int x, int y, int button, int bDown)
{
}

void HandleMotion(int x, int y, int mask)
{
}

#define HMX 40
#define HMY 40
short screenx, screeny;

void HandleDestroy()
{
	printf("Destroying\n");
	exit(10);
}

uint32_t randomtexturedata[65536];

void drawPixel(unsigned int x, unsigned int y, uint32_t color) {
	CNFGColor(0x444444FF);
	CNFGTackRectangle(x * PIXEL_SCALE, y * PIXEL_SCALE, (x + 1) * PIXEL_SCALE, (y + 1) * PIXEL_SCALE);
}

unsigned int main(int argc, char* argv[])
{
	char cwd[9999];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working dir: %s\n", cwd);
	}
	else {
		perror("getcwd() error");
	}

	// INITIALIZATION
	c8_rom_initialize();
	c8_ram_initialize();
	c8_cpu_initialize();
	c8_gpu_initialize();
	c8_kb_initialize();
	c8_buz_initialize();

	// SCHIP

	// CHIP8
	// Broken - Needs special instructions??
	//c8_rom_load("ROMS/bullet_hell.ch8");
	

	// Testing
	//c8_rom_load("ROMS/delay_timer_test.ch8");
	//c8_rom_load("ROMS/SCTEST.ch8"); I don't trust it
	//c8_rom_load("ROMS/random_number_test.ch8");
	//c8_rom_load("ROMS/test_opcode.ch8");
	//c8_rom_load("ROMS/c8_test.ch8");

	// completely functional
	//c8_rom_load("ROMS/Sirpinski.ch8");
	//c8_rom_load("ROMS/PONG.ch8");
	//c8_rom_load("ROMS/snake.ch8");
	//c8_rom_load("ROMS/1dcell.ch8");
	//c8_rom_load("ROMS/flightrunner.ch8");

	if (argc == 1) {

		c8_rom_load("ROMS/PONG.ch8");
	}
	else {
		printf("Rom: %s\n", argv[1]);
		c8_rom_load(argv[1]);
	}

	for (unsigned int byteIndex = 0; byteIndex < ROM_USED_SIZE; byteIndex++) {
		// Roms get stored from 0x200 to the end
		c8_ram_store_byte(c8_rom_fetch_byte(byteIndex), byteIndex + 0x200);
	}
	c8_ram_load_fonts();

	if (DEBUGGING){
		c8_rom_dump();
		c8_ram_dump();
	}

	// GRAPHICS

	double this_time;
	double LastFPSTime = OGGetAbsoluteTime();
	double last_tick_time = OGGetAbsoluteTime();
	double SecToWait;
	unsigned int linesegs = 0;

	CNFGBGColor = 0x00000000; //Black
	CNFGSetup("CChip-8", 64 * PIXEL_SCALE, 32 * PIXEL_SCALE);

	while (CNFGHandleInput())
	{

		c8_cpu_main_loop();

		if (ticks % (PROCESSOR_HZ /60) == 0) {
			// Counters 
			c8_cpu_dt_tick();
			c8_cpu_st_tick();
			c8_gpu_refresh_display();
		}
		ticks++;

		this_time = OGGetAbsoluteTime();
		if (this_time > LastFPSTime + 1)
		{
			printf("Hz: %d\n", ticks);
			ticks = 0;
			linesegs = 0;
			LastFPSTime += 1;
		}

		SecToWait = tick_time - (this_time - last_tick_time);
		last_tick_time += tick_time;
		if (SecToWait > 0)
			OGUSleep((unsigned int)(SecToWait * 1000000));
	}

	return(0);
}