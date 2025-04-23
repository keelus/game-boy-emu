#ifndef GB_BUS_H
#define GB_BUS_H

#include <stdint.h>
struct GB_Cpu;
struct GB_Ppu;
struct GB_Mem;

struct GB_Bus {
	struct GB_Cpu *cpu;
	struct GB_Ppu *ppu;
	struct GB_Mem *mem;
};
typedef struct GB_Bus GB_Bus;

enum GB_Bus_mem_caller {
	CALLER_CPU,
	CALLER_PPU,
};
typedef enum GB_Bus_mem_caller GB_Bus_mem_caller;

void GB_Bus_init(GB_Bus *bus);
void GB_Bus_free(GB_Bus *bus);

int8_t GB_Bus_mem_read(GB_Bus *bus, uint16_t address, GB_Bus_mem_caller caller);
void GB_Bus_mem_write(GB_Bus *bus, uint16_t address, uint8_t value,
					  GB_Bus_mem_caller caller);

#endif
