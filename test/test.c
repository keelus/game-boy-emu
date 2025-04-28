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

/* 0x00: NOP */
void test_0x00_nop(void) {
	bus.cpu->registers.PC = 0;

	bus.mem->data[0] = 0x00;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
}

/* 0x01: LD BC, n16 */
void test_0x01_ld_bc(void) {
	bus.cpu->registers.PC = 0;

	bus.mem->data[0] = 0x01;
	bus.mem->data[1] = 0x34;
	bus.mem->data[2] = 0x12;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(3, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x1234, bus.cpu->registers.BC);
}

/* 0x02: LD [BC], A */
void test_0x02_ld_bc(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.BC = 0x1234;
	bus.cpu->registers.A = 0x56;

	bus.mem->data[0] = 0x02;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x56, bus.mem->data[bus.cpu->registers.BC]);
}

/* 0x03: INC BC */
void test_0x03_inc_bc(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.BC = 0x1234;

	bus.mem->data[0] = 0x03;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x1235, bus.cpu->registers.BC);
}

void test_0x03_inc_bc_overflows(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.BC = 0xFFFF;
	bus.cpu->registers.F = 0x12;

	bus.mem->data[0] = 0x03;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0, bus.cpu->registers.BC);

	TEST_ASSERT_EQUAL(0x12, bus.cpu->registers.F); /* Flags remain unchanged */
}

/* 0x04: INC B */
void test_0x04_inc_b(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.B = 0x03;
	bus.cpu->registers.F = 0b00010000;

	bus.mem->data[0] = 0x04;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x4, bus.cpu->registers.B);

	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_ZERO));
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_SUB));
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_HALF_CARRY));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_CARRY, (bus.cpu->registers.F &
										  FLAG_CARRY)); /* Remains unchanged */
}

void test_0x04_inc_b_zero(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.B = 0xFF;
	bus.cpu->registers.F = 0b00010000;

	bus.mem->data[0] = 0x04;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0, bus.cpu->registers.B);

	TEST_ASSERT_EQUAL(0xF0 & FLAG_ZERO, (bus.cpu->registers.F & FLAG_ZERO));
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_SUB));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_HALF_CARRY,
					  (bus.cpu->registers.F & FLAG_HALF_CARRY));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_CARRY, (bus.cpu->registers.F &
										  FLAG_CARRY)); /* Remains unchanged */
}

void test_0x04_inc_b_half(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.B = 0x0F;
	bus.cpu->registers.F = 0b00010000;

	bus.mem->data[0] = 0x04;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0b00010000, bus.cpu->registers.B);

	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_ZERO));
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_SUB));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_HALF_CARRY,
					  (bus.cpu->registers.F & FLAG_HALF_CARRY));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_CARRY, (bus.cpu->registers.F &
										  FLAG_CARRY)); /* Remains unchanged */
}

/* 0x05: DEC B */
void test_0x05_dec_b(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.B = 0x12;
	bus.cpu->registers.F = 0b00010000;

	bus.mem->data[0] = 0x05;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x11, bus.cpu->registers.B);

	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_ZERO));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_SUB, (bus.cpu->registers.F & FLAG_SUB));
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_HALF_CARRY));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_CARRY, (bus.cpu->registers.F &
										  FLAG_CARRY)); /* Remains unchanged */
}

void test_0x05_dec_b_zero(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.B = 0x01;
	bus.cpu->registers.F = 0b00010000;

	bus.mem->data[0] = 0x05;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0, bus.cpu->registers.B);

	TEST_ASSERT_EQUAL(0xF0 & FLAG_ZERO, (bus.cpu->registers.F & FLAG_ZERO));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_SUB, (bus.cpu->registers.F & FLAG_SUB));
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_HALF_CARRY));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_CARRY, (bus.cpu->registers.F &
										  FLAG_CARRY)); /* Remains unchanged */
}

void test_0x05_dec_b_half(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.B = 0x10;
	bus.cpu->registers.F = 0b00010000;

	bus.mem->data[0] = 0x05;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x0F, bus.cpu->registers.B);

	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_ZERO));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_SUB, (bus.cpu->registers.F & FLAG_SUB));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_HALF_CARRY,
					  (bus.cpu->registers.F & FLAG_HALF_CARRY));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_CARRY, (bus.cpu->registers.F &
										  FLAG_CARRY)); /* Remains unchanged */
}

/* 0x06: LD B, n8 */
void test_0x06_ld_b(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.B = 0;

	bus.mem->data[0] = 0x06;
	bus.mem->data[1] = 0x12;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(2, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x12, bus.cpu->registers.B);
}

/* 0x07: RLCA */
// void test_0x07_rlca(void) {
// }

/* 0x08: LD [a16], SP */
void test_0x08_ld(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.SP = 0x1234;

	bus.mem->data[0] = 0x08;
	bus.mem->data[1] = 0x78;
	bus.mem->data[2] = 0x56;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(3, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x34, bus.mem->data[0x5678]);
	TEST_ASSERT_EQUAL(0x12, bus.mem->data[0x5678 + 1]);
}

/* 0x09: ADD HL, BC */
void test_0x09_add_hl(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.F = 0x80;
	bus.cpu->registers.HL = 0b0011010000101110;
	bus.cpu->registers.BC = 0b0110000100000001;

	bus.mem->data[0] = 0x09;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0b1001010100101111, bus.cpu->registers.HL);

	TEST_ASSERT_EQUAL(0xF0 & FLAG_ZERO, (bus.cpu->registers.F &
										 FLAG_ZERO)); /* Remains unchanged */
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_SUB));
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_HALF_CARRY));
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_CARRY));
}

void test_0x09_add_hl_carry(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.F = 0x80;
	bus.cpu->registers.HL = 0xFFFF;
	bus.cpu->registers.BC = 0x0001;

	bus.mem->data[0] = 0x09;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0, bus.cpu->registers.HL);

	TEST_ASSERT_EQUAL(0xF0 & FLAG_ZERO, (bus.cpu->registers.F &
										 FLAG_ZERO)); /* Remains unchanged */
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_SUB));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_HALF_CARRY,
					  (bus.cpu->registers.F & FLAG_HALF_CARRY));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_CARRY, (bus.cpu->registers.F & FLAG_CARRY));
}

void test_0x09_add_hl_half(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.F = 0x80;
	bus.cpu->registers.HL = 0x0F00;
	bus.cpu->registers.BC = 0x0100;

	bus.mem->data[0] = 0x09;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x1000, bus.cpu->registers.HL);

	TEST_ASSERT_EQUAL(0xF0 & FLAG_ZERO, (bus.cpu->registers.F &
										 FLAG_ZERO)); /* Remains unchanged */
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_SUB));
	TEST_ASSERT_EQUAL(0xF0 & FLAG_HALF_CARRY,
					  (bus.cpu->registers.F & FLAG_HALF_CARRY));
	TEST_ASSERT_EQUAL(0, (bus.cpu->registers.F & FLAG_CARRY));
}

/* 0x0A: LD A, [BC] */
void test_0x0A_add_a(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.BC = 0x1234;
	bus.cpu->registers.A = 0;

	bus.mem->data[0] = 0x0A;
	bus.mem->data[bus.cpu->registers.BC] = 0x56;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x56, bus.cpu->registers.A);
}

/* 0x0B: DEC BC */
void test_0x0B_dec_bc(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.BC = 0x1234;

	bus.mem->data[0] = 0x0B;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0x1233, bus.cpu->registers.BC);
}

void test_0x0B_dec_bc_underflows(void) {
	bus.cpu->registers.PC = 0;
	bus.cpu->registers.BC = 0;
	bus.cpu->registers.F = 0x12;

	bus.mem->data[0] = 0x0B;
	GB_Bus_tick(&bus);

	TEST_ASSERT_EQUAL(1, bus.cpu->registers.PC);
	TEST_ASSERT_EQUAL(0xFFFF, bus.cpu->registers.BC);

	TEST_ASSERT_EQUAL(0x12, bus.cpu->registers.F); /* Flags remain unchanged */
}

int main(void) {
	UNITY_BEGIN();

	RUN_TEST(test_0x00_nop);

	RUN_TEST(test_0x01_ld_bc);

	RUN_TEST(test_0x02_ld_bc);

	RUN_TEST(test_0x03_inc_bc);
	RUN_TEST(test_0x03_inc_bc_overflows);

	RUN_TEST(test_0x04_inc_b);
	RUN_TEST(test_0x04_inc_b_zero);
	RUN_TEST(test_0x04_inc_b_half);

	RUN_TEST(test_0x05_dec_b);
	RUN_TEST(test_0x05_dec_b_zero);
	RUN_TEST(test_0x05_dec_b_half);

	RUN_TEST(test_0x06_ld_b);

	RUN_TEST(test_0x08_ld);

	RUN_TEST(test_0x09_add_hl);
	RUN_TEST(test_0x09_add_hl_carry);
	RUN_TEST(test_0x09_add_hl_half);

	RUN_TEST(test_0x0A_add_a);

	RUN_TEST(test_0x0B_dec_bc);
	RUN_TEST(test_0x0B_dec_bc_underflows);

	return UNITY_END();
}
