#ifndef CPU_LOADED
	#define CPU_LOADED
	#undef RAND_MAX
	#define RAND_MAX 255
	#include "CPU.h"
	#include "GPU.h"
	#include "RAM.h"
	#include "Keyboard.h"
	#include "Global.h"
	#include <string.h>
	#include <time.h>
	#include <stdlib.h>
	#include <stdio.h>
	#if WINDOWS	== 1
		#include "Buzzer_win.c"
	#endif
	
	// We compare if the input OPcode is equal to the filter OPcode ignoring data bits.
	// if result = 0: Equal
	#define CHECK_BITS(inp, filter, data) ((inp|data) ^ (filter|data))
	#define INCREASE_PC (r_PC += 2)
	
	// 16 8Bit registers
	Byte r_Vx[16];

	// 1 16Bit register
	unsigned short r_I;

	// non accessable Program counter
	unsigned short r_PC;

	// Delay and sound timer 8 bit registers
	Byte r_DT;
	Byte r_ST;

	// Stack pointer
	Byte r_SP;

	// Stack
	unsigned short stack[16];
	unsigned short stack_size = 0;
	unsigned short c8_cpu_stack_pop() {
		if (stack_size == 0) {
			return 0;
		}
		else {
			stack_size -= 1;
			return stack[stack_size];
		}
	};
	void c8_cpu_stack_put(short val) {
		if (stack_size == 16) {
		}
		else {
			stack[stack_size] = val;
			stack_size += 1;
		}
	};

	void c8_cpu_main_loop() {
		unsigned short OPC = (* c8_ram_fetch(r_PC)) << 8 | (*c8_ram_fetch(r_PC+1));
		if (DEBUGGING == 1){
			printf("%x - %x\n", r_PC, OPC);
		}
		c8_cpu_execute_opc(OPC);
	}

	void c8_cpu_dt_tick() {
		if (r_DT > 0) {
			r_DT--;
		}
	}

	void c8_cpu_st_tick() {
		if (r_ST > 0) {
			r_ST--;
			c8_buz_tick();
		}
	}


	// 00E0
	void CLS(unsigned short input[]) {
		c8_gpu_clear();
		INCREASE_PC;
	}
	// 00EE
	void RET(unsigned short input[]) {
		r_PC = c8_cpu_stack_pop();
		r_SP -= 1;
		INCREASE_PC;
	}
	// 0nnn
	void SYS_addr(unsigned short input[]) {
		r_PC = input[0];
	}
	// 1nnn
	void JP_addr(unsigned short input[]) {
		r_PC = input[0];
	}
	// 2nnn
	void CALL_addr(unsigned short input[]) {

		r_SP += 1;
		c8_cpu_stack_put(r_PC);
		r_PC = input[0];
	}
	// 3xkk
	void SE_Vx_byte(unsigned short input[]) {
		if (r_Vx[input[0]]==input[1])
			r_PC += 2;
		INCREASE_PC;
	}
	// 4xkk
	void SNE_Vx_byte(unsigned short input[]) {
		if (r_Vx[input[0]] != input[1])
			r_PC += 2;
		INCREASE_PC;
	}
	// 5xy0
	void SE_Vx_Vy(unsigned short input[]) {
		if (r_Vx[input[0]] == r_Vx[input[1]])
			r_PC += 2;
		INCREASE_PC;
	}
	// 6xkk
	void LD_Vx_byte(unsigned short input[]) {
		r_Vx[input[0]] = input[1];
		INCREASE_PC;
	}
	// 7xkk
	void ADD_Vx_byte(unsigned short input[]) {
		r_Vx[input[0]] += input[1];
		INCREASE_PC;
	}
	// 8xy0
	void LD_Vx_Vy(unsigned short input[]) {
		r_Vx[input[0]] = r_Vx[input[1]];
		INCREASE_PC;
	}
	// 8xy1
	void OR_Vx_Vy(unsigned short input[]) {
		r_Vx[input[0]] |= r_Vx[input[1]];
		INCREASE_PC;
	}
	// 8xy2
	void AND_Vx_Vy(unsigned short input[]) {
		r_Vx[input[0]] &= r_Vx[input[1]];
		INCREASE_PC;
	}
	// 8xy3
	void XOR_Vx_Vy(unsigned short input[]) {
		r_Vx[input[0]] ^= r_Vx[input[1]];
		INCREASE_PC;
	}
	// 8xy4
	void ADD_Vx_Vy(unsigned short input[]) {
		short temp = r_Vx[input[0]] + r_Vx[input[1]];
		r_Vx[input[0]] = temp & 0xFF;
		r_VF = temp > 255 ? 1 : 0;
		INCREASE_PC;
	}
	// 8xy5
	void SUB_Vx_Vy(unsigned short input[]) {
		r_VF = (r_Vx[input[0]] > r_Vx[input[1]]) ? 1 : 0;
		r_Vx[input[0]] -= r_Vx[input[1]];
		INCREASE_PC;
	}
	// 8xy6
	void SHR_Vx_Vy(unsigned short input[]) {
		r_VF = r_Vx[input[0]] & 1;
		r_Vx[input[0]] >>= 1;
		INCREASE_PC;
	}
	// 8xy7
	void SUBN_Vx_Vy(unsigned short input[]) {
		r_VF = (r_Vx[input[1]] > r_Vx[input[0]]) ? 1 : 0;
		r_Vx[input[0]] = r_Vx[input[1]]-r_Vx[input[0]];
		INCREASE_PC;
	}
	// 8xyE
	void SHL_Vx_Vy(unsigned short input[]) {
		r_VF = r_Vx[input[0]]>>7 & 1;
		r_Vx[input[0]] <<= 1;
		INCREASE_PC;
	}
	// 9xy0
	void SNE_Vx_Vy(unsigned short input[]) {
		if (r_Vx[input[0]] != r_Vx[input[1]])
			r_PC += 2;
		INCREASE_PC;
	}
	// Annn
	void LD_I_addr(unsigned short input[]) {
		r_I = input[0];
		INCREASE_PC;
	}
	// Bnnn
	void JP_V0_addr(unsigned short input[]) {
		r_PC = input[0] + r_Vx[0];
	}
	// Cxkk
	void RND_Vx_byte(unsigned short input[]) {
		r_Vx[input[0]]=rand() & input[1];
		INCREASE_PC;
	}
	// Dxyn
	void DRW_Vx_Vy_nibble(unsigned short input[]) {
		r_VF = 0;
		Byte *sprite = (Byte * )malloc(input[2]);
		memcpy(sprite, c8_ram_fetch(r_I), input[2]);
		c8_gpu_display_sprite(input[0],input[1],sprite,input[2]);
		INCREASE_PC;
	}
	// Ex9E
	void SKP_Vx(unsigned short input[]) {
		if (c8_kb_is_pressed(r_Vx[input[0]]) == 1){
			r_PC += 2;
		}
		INCREASE_PC;
	}
	// ExA1
	void SKNP_Vx(unsigned short input[]) {
		if (! c8_kb_is_pressed(r_Vx[input[0]]) == 1){
			r_PC += 2;
		}
		INCREASE_PC;
	}
	// Fx07
	void LD_Vx_DT(unsigned short input[]) {
		
		r_Vx[input[0]] = r_DT;
		INCREASE_PC;
	}
	// Fx0A
	void LD_Vx_K(unsigned short input[]) {
		if (c8_kb_wait() == 1) {
			r_Vx[input[0]] == c8_kb_pop_wait_key();
			INCREASE_PC;
		}
		// TODO Reimplement
	}
	// Fx15
	void LD_DT_Vx(unsigned short input[]) {
		r_DT = r_Vx[input[0]];
		INCREASE_PC;
	}
	// Fx18
	void LD_ST_Vx(unsigned short input[]) {
		r_ST = r_Vx[input[0]];
		INCREASE_PC;
	}
	// Fx1E
	void ADD_I_Vx(unsigned short input[]) {
		r_I += r_Vx[input[0]];
		INCREASE_PC;
	}
	// Fx29
	void LD_F_Vx(unsigned short input[]) {
		r_I = r_Vx[input[0]] * 5;
		INCREASE_PC;
	}
	// Fx33
	void LD_B_Vx(unsigned short input[]) {
		Byte ones = (r_Vx[input[0]] - r_Vx[input[0]]/10*10);
		Byte tens = (r_Vx[input[0]] / 10 - r_Vx[input[0]] / 100 * 10);
		Byte hundreds = (r_Vx[input[0]] / 100);
		c8_ram_store_byte(hundreds, r_I);
		c8_ram_store_byte(tens, r_I+1);
		c8_ram_store_byte(ones, r_I+2);
		INCREASE_PC;
	}
	// Fx55
	void LD_I_Vx(unsigned short input[]) {
		for (int x=0;x<=input[0];x++){
			c8_ram_store_byte(r_Vx[x], r_I + x);
		}
		INCREASE_PC;
	}
	// Fx65
	void LD_Vx_I(unsigned short input[]) {
		for (int x = 0; x <= input[0]; x++) {
			r_Vx[x]=*c8_ram_fetch(r_I + x);
		}
		INCREASE_PC;
	}

	const fun_ptr OpCodes[35] = {
		&CLS,			&RET,			&SYS_addr,		&JP_addr,			&CALL_addr, 
		&SE_Vx_byte,	&SNE_Vx_byte,	&SE_Vx_Vy,		&LD_Vx_byte,		&ADD_Vx_byte,	
		&LD_Vx_Vy,		&OR_Vx_Vy,		&AND_Vx_Vy,		&XOR_Vx_Vy,			&ADD_Vx_Vy,		
		&SUB_Vx_Vy, 	&SHR_Vx_Vy,		&SUBN_Vx_Vy,	&SHL_Vx_Vy,			&SNE_Vx_Vy, 
		&LD_I_addr,		&JP_V0_addr,	&RND_Vx_byte,	&DRW_Vx_Vy_nibble, 	&SKP_Vx,		
		&SKNP_Vx,		&LD_Vx_DT,		&LD_Vx_K,		&LD_DT_Vx,			&LD_ST_Vx,
		&ADD_I_Vx,		&LD_F_Vx,		&LD_B_Vx,		&LD_I_Vx,			&LD_Vx_I
	};

	void c8_cpu_initialize() {
		memset(r_Vx, 0, 16);
		srand(time(NULL));
		r_PC = 0x200;
	}

	fun_ptr c8_cpu_execute_opc(unsigned short input) {
		// 00E0
		if (CHECK_BITS(input, 0b0000000011100000, 0b0000000000000000) == 0) {
			unsigned short in[] = { 0 };
			CLS(in);
			if (DEBUGGING == 1) {
				printf("inst: CLS\n");
			}
		}
		// 00EE
		else if (CHECK_BITS(input, 0b0000000011101110, 0b0000000000000000) == 0) {
			unsigned short in[] = { 0 };
			RET(in);
			if (DEBUGGING == 1) {
				printf("inst: RET\n");
			}
		}
		else if (CHECK_BITS(input, 0b0000000000000000, 0b0000111111111111) == 0) {
			// 0nnn
			unsigned short addr = input & 0b0000111111111111;
			unsigned short in[] = { addr };
			SYS_addr(in);
			if (DEBUGGING == 1) {
				printf("inst: SYS_addr\n");
			}
		}
		else if (CHECK_BITS(input, 0b0001000000000000, 0b0000111111111111) == 0) {
			// 1nnn
			unsigned short addr = input & 0b0000111111111111;
			unsigned short in[] = { addr };
			JP_addr(in);
			if (DEBUGGING == 1) {
				printf("inst: JP_addr\n");
			}
		}
		else if (CHECK_BITS(input, 0b0010000000000000, 0b0000111111111111) == 0) {
			// 2nnn
			unsigned short addr = input & 0b0000111111111111;
			unsigned short in[] = { addr };
			CALL_addr(in);
			if (DEBUGGING == 1) {
				printf("inst: CALL_addr\n");
			}
		}
		else if (CHECK_BITS(input, 0b0011000000000000, 0b0000111111111111) == 0) {
			// 3xkk
			unsigned short Vx = (input & 0b0000111100000000)>>8;
			unsigned short byte = input & 0b0000000011111111;
			unsigned short in[] = { Vx, byte};
			SE_Vx_byte(in);
			if (DEBUGGING == 1) {
				printf("inst: SE_Vx_byte\n");
			}
		}
		else if (CHECK_BITS(input, 0b0100000000000000, 0b0000111111111111) == 0) {
			// 4xkk
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short byte = input & 0b0000000011111111;
			unsigned short in[] = { Vx, byte };
			SNE_Vx_byte(in);
			if (DEBUGGING == 1) {
				printf("inst: SNE_Vx_byte\n");
			}
		}
		else if (CHECK_BITS(input, 0b0101000000000000, 0b0000111111111111) == 0) {
			// 5xy0
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			SE_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: SE_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b0110000000000000, 0b0000111111111111) == 0) {
			// 6xkk
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short byte = input & 0b0000000011111111;
			unsigned short in[] = { Vx, byte };
			LD_Vx_byte(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_Vx_byte\n");
			}
		}
		else if (CHECK_BITS(input, 0b0111000000000000, 0b0000111111111111) == 0) {
			// 7xkk
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short byte = input & 0b0000000011111111;
			unsigned short in[] = { Vx, byte };
			ADD_Vx_byte(in);
			if (DEBUGGING == 1) {
				printf("inst: ADD_Vx_byte\n");
			}
		}
		else if (CHECK_BITS(input, 0b1000000000000000, 0b0000111111110000) == 0) {
			// 8xy0
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			LD_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1000000000000001, 0b0000111111110000) == 0) {
			// 8xy1
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			OR_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: OR_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1000000000000010, 0b0000111111110000) == 0) {
			// 8xy2
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			AND_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: AND_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1000000000000011, 0b0000111111110000) == 0) {
			// 8xy3
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			XOR_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: XOR_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1000000000000100, 0b0000111111110000) == 0) {
			// 8xy4
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			ADD_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: ADD_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1000000000000101, 0b0000111111110000) == 0) {
			// 8xy5
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			SUB_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: SUB_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1000000000000110, 0b0000111111110000) == 0) {
			// 8xy6
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			SHR_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: SHR_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1000000000000111, 0b0000111111110000) == 0) {
			// 8xy7
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			SUBN_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: SUBN_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1000000000001110, 0b0000111111110000) == 0) {
			// 8xyE
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			SHL_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: SHL_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1001000000000000, 0b0000111111110000) == 0) {
			// 9xy0
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short in[] = { Vx, Vy };
			SNE_Vx_Vy(in);
			if (DEBUGGING == 1) {
				printf("inst: SNE_Vx_Vy\n");
			}
		}
		else if (CHECK_BITS(input, 0b1010000000000000, 0b0000111111111111) == 0) {
			// Annn
			unsigned short addr = input & 0b0000111111111111;
			unsigned short in[] = { addr };
			LD_I_addr(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_I_addr\n");
			}
		}
		else if (CHECK_BITS(input, 0b1011000000000000, 0b0000111111111111) == 0) {
			// Bnnn
			unsigned short addr = input & 0b0000111111111111;
			unsigned short in[] = { addr };
			JP_V0_addr(in);
			if (DEBUGGING == 1) {
				printf("inst: JP_V0_addr\n");
			}
		}
		else if (CHECK_BITS(input, 0b1100000000000000, 0b0000111111111111) == 0) {
			// Cxkk
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short byte = input & 0b0000000011111111;
			unsigned short in[] = { Vx, byte };
			RND_Vx_byte(in);
			if (DEBUGGING == 1) {
				printf("inst: RND_Vx_byte\n");
			}
		}
		else if (CHECK_BITS(input, 0b1101000000000000, 0b0000111111111111) == 0) {
			// Dxyn
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short Vy = (input & 0b0000000011110000) >> 4;
			unsigned short n = input & 0b0000000000001111;
			unsigned short in[] = { r_Vx[Vx], r_Vx[Vy], n};
			DRW_Vx_Vy_nibble(in);
			if (DEBUGGING == 1) {
				printf("inst: DRW_Vx_Vy_nibble\n");
			}
		}
		else if (CHECK_BITS(input, 0b1110000010011110, 0b0000111100000000) == 0) {
			// Ex9E
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			SKP_Vx(in);
			if (DEBUGGING == 1) {
				printf("inst: SKP_Vx\n");
			}
		}
		else if (CHECK_BITS(input, 0b1110000010100001, 0b0000111100000000) == 0) {
			// ExA1
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			SKNP_Vx(in);
			if (DEBUGGING == 1) {
				printf("inst: SKNP_Vx\n");
			}
		}
		else if (CHECK_BITS(input, 0b1111000000000111, 0b0000111100000000) == 0) {
			// Fx07
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			LD_Vx_DT(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_Vx_DT\n");
			}
		}
		else if (CHECK_BITS(input, 0b1111000000001010, 0b0000111100000000) == 0) {
			// Fx0A
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			LD_Vx_K(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_Vx_K\n");
			}
		}
		else if (CHECK_BITS(input, 0b1111000000010101, 0b0000111100000000) == 0) {
			// Fx15
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			LD_DT_Vx(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_DT_Vx\n");
			}
		}
		else if (CHECK_BITS(input, 0b1111000000011000, 0b0000111100000000) == 0) {
			// Fx18
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			LD_ST_Vx(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_ST_Vx\n");
			}
		}
		else if (CHECK_BITS(input, 0b1111000000011110, 0b0000111100000000) == 0) {
			// Fx1E
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			ADD_I_Vx(in);
			if (DEBUGGING == 1) {
				printf("inst: ADD_I_Vx\n");
			}
		}
		else if (CHECK_BITS(input, 0b1111000000101001, 0b0000111100000000) == 0) {
			// Fx29
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			LD_F_Vx(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_F_Vx\n");
			}
		}
		else if (CHECK_BITS(input, 0b1111000000110011, 0b0000111100000000) == 0) {
			// Fx33
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			LD_B_Vx(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_B_Vx\n");
			}
		}
		else if (CHECK_BITS(input, 0b1111000001010101, 0b0000111100000000) == 0) {
			// Fx55
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			LD_I_Vx(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_I_Vx\n");
			}
		}
		else if (CHECK_BITS(input, 0b1111000001100101, 0b0000111100000000) == 0) {
			// Fx65
			unsigned short Vx = (input & 0b0000111100000000) >> 8;
			unsigned short in[] = { Vx };
			LD_Vx_I(in);
			if (DEBUGGING == 1) {
				printf("inst: LD_Vx_I\n");
			}
		}
	}

#endif