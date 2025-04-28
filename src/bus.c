#include "bus.h"
#include "mem.h"
#include "cpu.h"
#include "ppu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void GB_Bus_init(GB_Bus *bus) {
	bus->cpu = (GB_Cpu *)malloc(sizeof(GB_Cpu));
	GB_Cpu_init(bus->cpu, bus);

	bus->mem = (GB_Mem *)malloc(sizeof(GB_Mem));
	GB_Mem_init(bus->mem, bus);

	bus->ppu = (GB_Ppu *)malloc(sizeof(GB_Ppu));
	// GB_Ppu_init(bus->ppu, bus);
}

void GB_Bus_free(GB_Bus *bus) {
	free(bus->cpu);
	free(bus->mem);
	free(bus->ppu);
}

bool GB_Mem_is_cpu_accessible(GB_Bus *bus, uint16_t address) {
	if(address >= GB_MEM_VRAM_BEGIN && address <= GB_MEM_VRAM_END &&
	   bus->ppu->state == PIXEL_TRANSFER) {
		return false;
	}

	if(address >= GB_MEM_OAM_BEGIN && address <= GB_MEM_OAM_END &&
	   (bus->ppu->state == OAM_SEARCH || bus->ppu->state == PIXEL_TRANSFER)) {
		return false;
	}

	return true;
}


int8_t GB_Bus_mem_read(GB_Bus *bus, uint16_t address,
					   GB_Bus_mem_caller caller) {
	if(!GB_Mem_is_cpu_accessible(bus, address) && caller == CALLER_CPU) {
		fprintf(stderr,
				"[WARN] CPU attempted to read from locked VRAM/OAM region.\n");
		return 0xFF;
	}

	return GB_Mem_read(bus->mem, address);
}

void GB_Bus_mem_write(GB_Bus *bus, uint16_t address, uint8_t value,
					  GB_Bus_mem_caller caller) {
	if(!GB_Mem_is_cpu_accessible(bus, address) && caller == CALLER_CPU) {
		fprintf(stderr,
				"[WARN] CPU attempted to write to locked VRAM/OAM region.\n");
		return;
	}

	GB_Mem_write(bus->mem, address, value);
}

void GB_Bus_tick(GB_Bus *bus) {
	uint8_t cycles = GB_Cpu_tick(bus->cpu);

	// Advance CPU
	// Advance PPU
}
