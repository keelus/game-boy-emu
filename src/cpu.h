#ifndef GB_CPU_H
#define GB_CPU_H

#include "bus.h"
#include <stdint.h>

#define FLAG_ZERO 0x80
#define FLAG_SUB 0x40
#define FLAG_HALF_CARRY 0x20
#define FLAG_CARRY 0x10

struct GB_Cpu {
	GB_Bus *bus;

	struct {
		uint8_t A;
		uint8_t F;

		uint8_t B;
		uint8_t C;

		uint8_t D;
		uint8_t E;

		uint8_t H;
		uint8_t L;

		uint16_t SP;
		uint16_t PC;
	} registers;
};
typedef struct GB_Cpu GB_Cpu;

void GB_Cpu_init(GB_Cpu *cpu);

#endif
