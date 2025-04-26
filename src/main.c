#include "bus.h"

int main(void) {
	GB_Bus gb;
	GB_Bus_init(&gb);

	while(1) {
		GB_Bus_tick(&gb);
	}

	GB_Bus_free(&gb);
	return 0;
}
