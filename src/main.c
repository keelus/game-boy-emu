#include "bus.h"
#include "mem.h"

int main(void) {
	GB_Bus gb;
	GB_Bus_init(&gb);
	if(GB_Mem_load_rom(gb.mem, "../roms/tetris.gb") != 0) { return -1; }

	while(1) {
		GB_Bus_tick(&gb);
	}

	GB_Bus_free(&gb);
	return 0;
}
