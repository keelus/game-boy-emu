#ifndef GB_MEM_H
#define GB_MEM_H

#include "bus.h"
#include <stdint.h>

#define GB_MEM_SIZE 65536

#define GB_MEM_VRAM_BEGIN 0x8000
#define GB_MEM_VRAM_END 0x9FFF
#define GB_MEM_OAM_BEGIN 0xFE00
#define GB_MEM_OAM_END 0xFE9F

struct GB_Mem {
	GB_Bus *bus;
	uint8_t data[GB_MEM_SIZE];
};
typedef struct GB_Mem GB_Mem;

void GB_Mem_init(GB_Mem *mem, GB_Bus *bus);

uint8_t GB_Mem_read(GB_Mem *mem, uint16_t address);
void GB_Mem_write(GB_Mem *mem, uint16_t address, uint8_t value);

#endif
