#include "cpu.h"
#include "bus.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void GB_Cpu_init(GB_Cpu *cpu, GB_Bus *bus) {
	cpu->bus = bus;

	cpu->IME = false;

	cpu->registers.AF = 0;
	cpu->registers.BC = 0;
	cpu->registers.DE = 0;
	cpu->registers.HL = 0;

	cpu->registers.PC = 0x100;
	cpu->registers.SP = 0;
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
		cpu->registers.C = lsb;
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
	case 0x07: /* RLCA */ {
		uint8_t a = cpu->registers.A;
		uint8_t result = (a << 1) | (a >> 7);

		GB_Cpu_set_flag(cpu, FLAG_ZERO, 0);
		GB_Cpu_set_flag(cpu, FLAG_SUB, 0);
		GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, 0);
		GB_Cpu_set_flag(cpu, FLAG_CARRY, result & 0x01);
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
		uint16_t hl = cpu->registers.HL;
		uint16_t bc = cpu->registers.BC;

		uint16_t result = hl + bc;

		bool half_carry = (hl & 0x07FF) + (bc & 0x07FF) > 0x07FF;
		bool carry = result < hl;

		GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, half_carry);
		GB_Cpu_set_flag(cpu, FLAG_CARRY, carry);
		GB_Cpu_set_flag(cpu, FLAG_SUB, 0);

		cpu->registers.HL = result;
	} break;
	case 0x0A: /* LD A, [BC] */ {
		cpu->registers.A =
			GB_Bus_mem_read(cpu->bus, cpu->registers.BC, CALLER_CPU);
	} break;
	case 0x0B: /* DEC BC */ {
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
	// case 0x0F: /* RRCA */ {
	// } break;
	case 0x10: /* STOP n8 */ {
		GB_Bus_mem_read(cpu->bus, cpu->registers.PC++,
						CALLER_CPU); /* Ignore next byte */
	} break;
	case 0x11: /* LD DE, n16 */ {
		uint8_t lsb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		uint8_t msb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);

		cpu->registers.D = msb;
		cpu->registers.E = lsb;
	} break;
	case 0x12: /* LD [DE], A */ {
		GB_Bus_mem_write(cpu->bus, cpu->registers.DE, cpu->registers.A,
						 CALLER_CPU);
	} break;
	case 0x13: /* INC DE */ {
		cpu->registers.DE++;
	} break;
	case 0x14: /* INC D */ {
		cpu->registers.D = GB_Cpu_inc_8reg_and_set_flags(cpu, cpu->registers.D);
	} break;
	case 0x15: /* DEC D */ {
		cpu->registers.D = GB_Cpu_dec_8reg_and_set_flags(cpu, cpu->registers.D);
	} break;
	case 0x16: /* LD D, n8 */ {
		cpu->registers.D =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
	} break;
	// case 0x17: /* RLA */ {
	// } break;
	case 0x18: /* JR e8 */ {
		uint8_t e = GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		cpu->registers.PC += e;
	} break;
	case 0x19: /* ADD HL, DE */ {
		uint16_t hl = cpu->registers.HL;
		uint16_t de = cpu->registers.DE;

		uint16_t result = hl + de;

		bool half_carry = (hl & 0x07FF) + (de & 0x07FF) > 0x07FF;
		bool carry = result < hl;

		GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, half_carry);
		GB_Cpu_set_flag(cpu, FLAG_CARRY, carry);
		GB_Cpu_set_flag(cpu, FLAG_SUB, 0);

		cpu->registers.HL = result;
	} break;
	case 0x1A: /* LD A, [DE] */ {
		cpu->registers.A =
			GB_Bus_mem_read(cpu->bus, cpu->registers.DE, CALLER_CPU);
	} break;
	case 0x1B: /* DEC DE */ {
		cpu->registers.DE--;
	} break;
	case 0x1C: /* INC E */ {
		cpu->registers.E = GB_Cpu_inc_8reg_and_set_flags(cpu, cpu->registers.E);
	} break;
	case 0x1D: /* DEC E */ {
		cpu->registers.E = GB_Cpu_dec_8reg_and_set_flags(cpu, cpu->registers.E);
	} break;
	case 0x1E: /* LD E, n8 */ {
		cpu->registers.E =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
	} break;
	// case 0x1F: /* RRA */ {
	// } break;
	case 0x20: /* JR NZ, e8 */ {
		int8_t e = GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);

		if(!GB_Cpu_get_flag(cpu, FLAG_ZERO)) { cpu->registers.PC += e; }
	} break;
	case 0x21: /* LD HL, n16 */ {
		uint8_t lsb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		uint8_t msb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);

		cpu->registers.H = msb;
		cpu->registers.L = lsb;
	} break;
	case 0x22: /* LD [HL+], A */ {
		GB_Bus_mem_write(cpu->bus, cpu->registers.HL++, cpu->registers.A,
						 CALLER_CPU);
	} break;
	case 0x2E: /* LD L, n8 */ {
		cpu->registers.L =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
	} break;
	case 0x30: /* JR NC, e8 */ {
		int8_t e = GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);

		if(!GB_Cpu_get_flag(cpu, FLAG_CARRY)) { cpu->registers.PC += e; }
	} break;
	case 0x31: /* LD SP, n16 */ {
		uint8_t lsb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		uint8_t msb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);

		cpu->registers.SP = (msb << 8) | lsb;
	} break;
	case 0x32: /* LD [HL-], A */ {
		GB_Bus_mem_write(cpu->bus, cpu->registers.HL--, cpu->registers.A,
						 CALLER_CPU);
	} break;
	case 0x3E: /* LD A, n8 */ {
		cpu->registers.A =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
	} break;
	case 0x80: /* ADD A, B */ {
		cpu->registers.A =
			GB_Cpu_add_and_set_flags(cpu, cpu->registers.A, cpu->registers.B);
	} break;
	case 0xC3: /* JP a16 */ {
		uint8_t lsb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		uint8_t msb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);

		cpu->registers.PC = (msb << 8) | lsb;
		uint16_t sp = cpu->registers.SP;
	} break;
	case 0xAF: /* XOR A, A */ {
		cpu->registers.A =
			GB_Cpu_xor_and_set_flags(cpu, cpu->registers.A, cpu->registers.A);
	} break;
	case 0xE0: /* LDH [a8], A */ {
		uint8_t lsb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		uint16_t addr = (0xFF << 8) | lsb;
		GB_Bus_mem_write(cpu->bus, addr, cpu->registers.A, CALLER_CPU);
	} break;
	case 0xF0: /* LDH A, [a8] */ {
		uint8_t lsb =
			GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		uint16_t addr = (0xFF << 8) | lsb;
		cpu->registers.A = GB_Bus_mem_read(cpu->bus, addr, CALLER_CPU);
	} break;
	case 0xF3: /* DI */ {
		cpu->IME = false;
	} break;
	case 0xFB: /* EI */ {
		cpu->IME = true;
	} break;
	case 0xFE: /* CP A, n8 */ {
		uint8_t n = GB_Bus_mem_read(cpu->bus, cpu->registers.PC++, CALLER_CPU);
		GB_Cpu_sub_and_set_flags(cpu, cpu->registers.A, n);
	} break;
	default:
		fprintf(
			stderr,
			"[ERROR] Missing instruction for the Opcode 0x%02X (at 0x%04X).\n",
			opcode, cpu->registers.PC - 1);
		exit(-1);
		break;
	}

	return cycles;
}

bool GB_Cpu_get_flag(GB_Cpu *cpu, uint8_t mask) {
	return (cpu->registers.F & mask) != 0;
}

void GB_Cpu_set_flag(GB_Cpu *cpu, uint8_t mask, bool on) {
	if(on)
		cpu->registers.F |= mask;
	else
		cpu->registers.F &= ~mask;
}

uint8_t GB_Cpu_add_and_set_flags(GB_Cpu *cpu, uint8_t a, uint8_t b) {
	uint8_t result = a + b;

	bool half_carry = (a & 0xF) + (b & 0xF) > 0xF;
	bool carry = result < a;

	GB_Cpu_set_flag(cpu, FLAG_ZERO, result == 0);
	GB_Cpu_set_flag(cpu, FLAG_SUB, 0);
	GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, half_carry);
	GB_Cpu_set_flag(cpu, FLAG_CARRY, carry);

	return result;
}

uint8_t GB_Cpu_sub_and_set_flags(GB_Cpu *cpu, uint8_t a, uint8_t b) {
	uint8_t result = a - b;

	bool half_carry = (a & 0xF) + (b & 0xF) > 0xF;
	bool carry = result < a;

	GB_Cpu_set_flag(cpu, FLAG_ZERO, result == 0);
	GB_Cpu_set_flag(cpu, FLAG_SUB, 1);
	GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, half_carry);
	GB_Cpu_set_flag(cpu, FLAG_CARRY, carry);

	return result;
}

uint8_t GB_Cpu_inc_8reg_and_set_flags(GB_Cpu *cpu, uint8_t value) {
	uint8_t result = value + 1;

	GB_Cpu_set_flag(cpu, FLAG_ZERO, result == 0);
	GB_Cpu_set_flag(cpu, FLAG_SUB, 0);
	GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, (value & 0x0F) == 0x0F);

	return result;
}

uint8_t GB_Cpu_dec_8reg_and_set_flags(GB_Cpu *cpu, uint8_t value) {
	uint8_t result = value - 1;

	GB_Cpu_set_flag(cpu, FLAG_ZERO, result == 0);
	GB_Cpu_set_flag(cpu, FLAG_SUB, 1);
	GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, (value & 0x0F) == 0x00);

	return result;
}

uint8_t GB_Cpu_xor_and_set_flags(GB_Cpu *cpu, uint8_t a, uint8_t b) {
	uint8_t result = a ^ b;

	GB_Cpu_set_flag(cpu, FLAG_ZERO, result == 0);
	GB_Cpu_set_flag(cpu, FLAG_SUB, 0);
	GB_Cpu_set_flag(cpu, FLAG_HALF_CARRY, 0);
	GB_Cpu_set_flag(cpu, FLAG_CARRY, 0);

	return result;
}
