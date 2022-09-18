#pragma once
#include "Global.h"
#define r_VF (r_Vx[0xF])
typedef void (*fun_ptr)(unsigned short input[]);

extern const fun_ptr OpCodes[35];

// 16 8Bit registers
Byte r_Vx[16];

// 1 16Bit register
unsigned short r_I;

// non accessable Program counter
unsigned short r_PC;

// Delay and sound timer 8 bit registers
Byte r_DT;
extern Byte r_ST;

// Stack pounsigned inter
Byte r_SP;

// Stack
unsigned short stack[16];
unsigned short stack_size;

fun_ptr c8_cpu_execute_opc(unsigned short input);
void c8_cpu_main_loop();
void c8_cpu_dt_tick();

void c8_cpu_st_tick();