#include "cpu.h"
#include "bus.h"
#include <stdint.h>
#include <stdio.h>

void GB_Cpu_init(GB_Cpu *cpu, GB_Bus *bus) {
}

// clang-format off
uint8_t CYCLE_TABLE[0x100] = {
 /*        x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */
 /* x0 */   4, 12,  8,  8,  4,  4,  8,  4, 20,  8,  8,  8,  4,  4,  4,  4,
 /* x1 */   4, 12,  8,  8,  4,  4,  8,  4, 12,  8,  8,  8,  4,  4,  8,  4,
 /* x2 */   8, 12,  8,  8,  4,  4,  8,  4,  8,  8,  8,  8,  4,  4,  8,  4,
 /* x3 */   8, 12,  8,  8, 12, 12, 12,  4,  8,  8,  8,  8,  4,  4,  8,  4,
 /* x4 */   4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
 /* x5 */   4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
 /* x6 */   4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
 /* x7 */   8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4,
 /* x8 */   4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
 /* x9 */   4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
 /* xA */   4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
 /* xB */   4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
 /* xC */   8, 12, 12, 16, 12, 16,  8, 16,  8, 16, 12,  4, 12, 24,  8, 16,
 /* xD */   8, 12, 12,  0, 12, 16,  8, 16,  8, 16, 12,  0, 12,  0,  8, 16,
 /* xE */  12, 12,  8,  0,  0, 16,  8, 16, 16,  4, 16,  0,  0,  0,  8, 16,
 /* xF */  12, 12,  8,  4,  0, 16,  8, 16, 12,  8, 16,  4,  0,  0,  8, 16
};
// clang-format on

/*
 * Instructions that need extra cycles if conditions met:
 * x20: 4  cycles
 * x28: 4  cycles
 *
 * x30: 4  cycles
 * x38: 4  cycles
 *
 * xC0: 12 cycles
 * xC2: 4  cycles
 * xC4: 12 cycles
 * xC8: 12 cycles
 * xCA: 4  cycles
 * xCC: 12 cycles
 *
 * xD0: 12 cycles
 * xD2: 4  cycles
 * xD4: 12 cycles
 * xD8: 12 cycles
 * xDA: 4  cycles
 * xDC: 12 cycles
 */

uint8_t GB_Cpu_tick(GB_Cpu *cpu) {
	uint8_t opcode = GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
	uint8_t cycles = CYCLE_TABLE[opcode];

	switch(opcode) {
	default:
		fprintf(stderr, "[WARN] Missing instruction for the Opcode %x.\n",
				opcode);
		break;
	}

	return cycles;
}
