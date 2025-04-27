#include "mem.h"
#include <stdbool.h>
#include <stdint.h>

void GB_Mem_init(GB_Mem *mem, GB_Bus *bus) {
}

uint8_t GB_Mem_read(GB_Mem *mem, uint16_t address) {
	return mem->data[address];
}

void GB_Mem_write(GB_Mem *mem, uint16_t address, uint8_t value) {
	mem->data[address] = value;
}
