#include "bus.h"
#include "unity.h"
#include "cpu.h"
#include "mem.h"

GB_Bus bus;

void setUp(void) {
	GB_Bus_init(&bus);
}

void tearDown(void) {
}

void test_inc_bc(void) {
	TEST_ASSERT_EQUAL(4, 2 + 2);
	GB_Bus bus;
	GB_Bus_init(&bus);

	bus.cpu->registers.PC = 0;
	bus.cpu->registers.BC = 0;
	bus.mem->data[0] = 0x03;
	GB_Cpu_tick(bus.cpu);

	TEST_ASSERT_EQUAL(bus.cpu->registers.BC, 1);
	TEST_ASSERT_EQUAL(bus.cpu->registers.PC, 1);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_inc_bc);
	return UNITY_END();
}
