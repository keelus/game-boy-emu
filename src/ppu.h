#ifndef GB_PPU_H
#define GB_PPU_H

#include "bus.h"
enum GB_Ppu_State {
	H_BLANK = 0,		/* OAM & VRAM directly accessible */
	V_BLANK = 1,		/* OAM & VRAM directly accessible */
	OAM_SEARCH = 2,		/* VRAM directly accessible. OAM innaccessible */
	PIXEL_TRANSFER = 3, /* OAM & VRAM are directly innaccessible */
};
typedef enum GB_Ppu_State GB_Ppu_State;

struct GB_Ppu {
	GB_Bus *bus;
	GB_Ppu_State state;
};
typedef struct GB_Ppu GB_Ppu;

void GB_Ppu_init(GB_Ppu *ppu, GB_Bus *bus);

#endif
