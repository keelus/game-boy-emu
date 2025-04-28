#include "cpu.h"
#include "bus.h"
#include <stdbool.h>
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
	case 0x00: /* NOP */ break;
	case 0x01: /* LD BC, n16 */ {
		uint8_t lsb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		uint8_t msb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);

		cpu->registers.B = msb;
		cpu->registers.C = msb;
	} break;
	case 0x02: /* LD [BC], A */ {
		GB_Bus_mem_write(cpu->bus, cpu->registers.BC, cpu->registers.A,
						 CALLER_CPU);
	} break;
	case 0x03: /* INC BC */ {
		cpu->registers.BC++;
	} break;
	case 0x04: /* INC B */ {
		cpu->registers.B = GB_Cpu_inc_8reg_and_set_flags(cpu, cpu->registers.B);
	} break;
	case 0x05: /* DEC B */ {
		cpu->registers.B = GB_Cpu_dec_8reg_and_set_flags(cpu, cpu->registers.B);
	} break;
	case 0x06: /* LD B, n8 */ {
		cpu->registers.B =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
	} break;
	case 0x07: /* RCLA */ {
		uint8_t a = cpu->registers.A;
		uint8_t result = (a << 1) | (a >> 7);

		GB_Cpu_set_flag(cpu, FLAG_ZERO, 0);
		GB_Cpu_set_flag(cpu, FLAG_SUB, 0);
		GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, 0);
		GB_Cpu_set_flag(cpu, FLAG_CARRY, a & 0x01);
	} break;
	case 0x08: /* LD [a16], SP */ {
		uint8_t lsb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		uint8_t msb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);

		uint16_t addr = (msb << 8) | lsb;
		uint16_t sp = cpu->registers.SP;

		GB_Bus_mem_write(cpu->bus, addr, sp & 0xFF, CALLER_CPU);
		GB_Bus_mem_write(cpu->bus, addr + 1, sp >> 8, CALLER_CPU);
	} break;
	case 0x09: /* ADD HL, BC */ {
		uint8_t l = GB_Cpu_add_and_set_flags(cpu, cpu->registers.L,
											 cpu->registers.C, false);
		uint8_t h = GB_Cpu_add_and_set_flags(cpu, cpu->registers.H,
											 cpu->registers.B, false);

		cpu->registers.L = l;
		cpu->registers.H = h;

		GB_Cpu_set_flag(cpu, FLAG_SUB, 0);
	} break;
	case 0x0A: /* LD A, [BC] */ {
		cpu->registers.A =
			GB_Bus_mem_read(cpu->bus, cpu->registers.BC, CALLER_CPU);
	} break;
	case 0x0B: /* DEC, BC */ {
		cpu->registers.BC--;
	} break;
	case 0x0C: /* INC C */ {
		cpu->registers.C = GB_Cpu_inc_8reg_and_set_flags(cpu, cpu->registers.C);
	} break;
	case 0x0D: /* DEC C */ {
		cpu->registers.C = GB_Cpu_dec_8reg_and_set_flags(cpu, cpu->registers.C);
	} break;
	case 0x0E: /* LD C, n8 */ {
		cpu->registers.C =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
	} break;
	default:
		fprintf(stderr, "[WARN] Missing instruction for the Opcode %x.\n",
				opcode);
		break;
	}

	return cycles;
}

void GB_Cpu_set_flag(GB_Cpu *cpu, uint8_t mask, bool on) {
	if(on)
		cpu->registers.F |= mask;
	else
		cpu->registers.F &= ~mask;
}

uint8_t GB_Cpu_add_and_set_flags(GB_Cpu *cpu, uint8_t a, uint8_t b,
								 bool update_zero_flag) {
	uint8_t result = a + b;

	uint8_t half_carry = ((a & 0xF) + (b & 0xF)) > 0xF;
	uint8_t carry = result < a;

	GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, half_carry);
	GB_Cpu_set_flag(cpu, FLAG_CARRY, carry);

	if(update_zero_flag) GB_Cpu_set_flag(cpu, FLAG_ZERO, result == 0);
}

uint8_t GB_Cpu_inc_8reg_and_set_flags(GB_Cpu *cpu, uint8_t value) {
	uint8_t result = value + 1;

	GB_Cpu_set_flag(cpu, FLAG_ZERO, result == 0);
	GB_Cpu_set_flag(cpu, FLAG_SUB, 0);
	GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, result < value);

	return result;
}

uint8_t GB_Cpu_dec_8reg_and_set_flags(GB_Cpu *cpu, uint8_t value) {
	uint8_t result = value - 1;

	GB_Cpu_set_flag(cpu, FLAG_ZERO, result == 0);
	GB_Cpu_set_flag(cpu, FLAG_SUB, 1);
	GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, result > value);

	return result;
}
