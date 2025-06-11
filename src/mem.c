#include "mem.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void GB_Mem_init(GB_Mem *mem, GB_Bus *bus) {
	mem->bus = bus;

	memset(mem->data, 0, sizeof(mem->data));
}

int GB_Mem_load_rom(GB_Mem *mem, char *rom_path) {
	FILE *f = fopen(rom_path, "rb");
	if(f == NULL) {
		fprintf(stderr, "Could not read ROM located at \"%s\".\n", rom_path);
		return -1;
	}

	size_t rom_length = fread((void *)mem->data, 1, GB_MEM_SIZE, f);
	fclose(f);


	uint8_t rom_type = mem->data[0x147];
	if(rom_type != 0) {
		fprintf(stderr, "Only ROM ONLY cartridge types allowed (%d detected).",
				rom_type);
		return -1;
	}

	printf("Loaded ROM of size %lukB.\n", rom_length / 1024);
	return 0;
}

uint8_t GB_Mem_read(GB_Mem *mem, uint16_t address) {
	return mem->data[address];
}

void GB_Mem_write(GB_Mem *mem, uint16_t address, uint8_t value) {
	mem->data[address] = value;
}
