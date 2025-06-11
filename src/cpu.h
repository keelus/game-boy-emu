#ifndef GB_CPU_H
#define GB_CPU_H

#include "bus.h"
#include <stdbool.h>
#include <stdint.h>

#define FLAG_ZERO 0x80
#define FLAG_SUB 0x40
#define FLAG_HALF_CARRY 0x20
#define FLAG_CARRY 0x10

struct GB_Cpu {
	GB_Bus *bus;

	struct {
		union {
			uint16_t AF;

			struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				uint8_t F;
				uint8_t A;
#else
				uint8_t A;
				uint8_t F;
#endif
			};
		};

		union {
			uint16_t BC;

			struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				uint8_t C;
				uint8_t B;
#else
				uint8_t B;
				uint8_t C;
#endif
			};
		};

		union {
			uint16_t DE;

			struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				uint8_t E;
				uint8_t D;
#else
				uint8_t D;
				uint8_t E;
#endif
			};
		};

		union {
			uint16_t HL;

			struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				uint8_t L;
				uint8_t H;
#else
				uint8_t H;
				uint8_t L;
#endif
			};
		};

		uint16_t SP;
		uint16_t PC;
	} registers;
	bool IME;
};
typedef struct GB_Cpu GB_Cpu;

void GB_Cpu_init(GB_Cpu *cpu, GB_Bus *bus);
uint8_t GB_Cpu_tick(GB_Cpu *cpu);
bool GB_Cpu_get_flag(GB_Cpu *cpu, uint8_t mask);
void GB_Cpu_set_flag(GB_Cpu *cpu, uint8_t mask, bool value);

uint8_t GB_Cpu_add_and_set_flags(GB_Cpu *cpu, uint8_t a, uint8_t b);
uint8_t GB_Cpu_sub_and_set_flags(GB_Cpu *cpu, uint8_t a, uint8_t b);

uint8_t GB_Cpu_inc_8reg_and_set_flags(GB_Cpu *cpu, uint8_t value);
uint8_t GB_Cpu_dec_8reg_and_set_flags(GB_Cpu *cpu, uint8_t value);
uint8_t GB_Cpu_xor_and_set_flags(GB_Cpu *cpu, uint8_t a, uint8_t b);

#endif
