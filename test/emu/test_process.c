#include "unity.h"
#include "process.h"
#include "vm.h"

#define TEST_VM_SIZE 4096*4
#define S(X) (sizeof(X)/sizeof(X[0]))
#define CODE(...) do { \
    u8 code[] = { __VA_ARGS__ }; \
    setup_vm(&vm, code, S(code)); \
} while(0)
#define P_R_REPEAT(TIMES, PROC_TYPE) do { \
    int i; \
    for(i=0; i<TIMES;i++) { \
      PROC_TYPE(get_instruction(vm.code[vm.regs.pc]), &vm, false); \
      vm.regs.pc++; \
    } \
} while(0)


vm_t vm;

void setUp(void) {
}

void tearDown(void) {
}

void test_is_valid_reg(void) {
    TEST_ASSERT_EQUAL(is_valid_reg(RR1), true);
    TEST_ASSERT_EQUAL(is_valid_reg(RR2), true);
    TEST_ASSERT_EQUAL(is_valid_reg(RR3), true);
    TEST_ASSERT_EQUAL(is_valid_reg(RR4), true);
    TEST_ASSERT_EQUAL(is_valid_reg(RR5), true);
    TEST_ASSERT_EQUAL(is_valid_reg(RR6), true);
    TEST_ASSERT_EQUAL(is_valid_reg(RR7), true);
    TEST_ASSERT_EQUAL(is_valid_reg(RR8), true);
    TEST_ASSERT_EQUAL(is_valid_reg(RPS), true);
    TEST_ASSERT_EQUAL(is_valid_reg(RPC), true);
}

void setup_vm(vm_t *vm, u8 *code, int sz) {
  term_vm(vm);
  init_vm(vm, TEST_VM_SIZE, TEST_VM_SIZE);
  memcpy(vm->code, code, sz);
}

void test_op_movid(void) {
    u8 code[] = {
        OP_MOVID, RR1, 0x11, 0x22, 0x33, 0x44,
        OP_MOVID, RR2, 0x11, 0x22, 0x33, 0x44,
        OP_MOVID, RR3, 0x11, 0x22, 0x33, 0x44,
        OP_MOVID, RR4, 0x11, 0x22, 0x33, 0x44,
    };
    setup_vm(&vm, code, S(code));
    P_R_REPEAT(4, process_reg_immd);
    TEST_ASSERT_EQUAL(vm.regs.r1, 0x11223344);
    TEST_ASSERT_EQUAL(vm.regs.r2, 0x11223344);
    TEST_ASSERT_EQUAL(vm.regs.r3, 0x11223344);
    TEST_ASSERT_EQUAL(vm.regs.r4, 0x11223344);
}


void test_op_addid(void) {
    CODE (
        OP_ADDID, RR1, 0x11, 0x22, 0x33, 0x44,
        OP_ADDID, RR2, 0x11, 0x22, 0x33, 0x44,
        OP_ADDID, RR3, 0x11, 0x22, 0x33, 0x44,
        OP_ADDID, RR4, 0x11, 0x22, 0x33, 0x44
        );
    P_R_REPEAT(4, process_reg_immd);
    TEST_ASSERT_EQUAL(vm.regs.r1, 0x11223344);
    TEST_ASSERT_EQUAL(vm.regs.r2, 0x11223344);
    TEST_ASSERT_EQUAL(vm.regs.r3, 0x11223344);
    TEST_ASSERT_EQUAL(vm.regs.r4, 0x11223344);
}

void test_op_subid(void) {
    CODE (
        OP_SUBID, RR1, 0x11, 0x22, 0x33, 0x44,
        OP_SUBID, RR2, 0x11, 0x22, 0x33, 0x44,
        OP_SUBID, RR3, 0x11, 0x22, 0x33, 0x44,
        OP_SUBID, RR4, 0x11, 0x22, 0x33, 0x44
        );
    P_R_REPEAT(4, process_reg_immd);
    TEST_ASSERT_EQUAL(vm.regs.r1, 0xeeddccbc);
    TEST_ASSERT_EQUAL(vm.regs.r2, 0xeeddccbc);
    TEST_ASSERT_EQUAL(vm.regs.r3, 0xeeddccbc);
    TEST_ASSERT_EQUAL(vm.regs.r4, 0xeeddccbc);
}

void test_op_add(void) {
    u8 code[] = {
        OP_ADDIB, RR1, 0x01,
        OP_ADDIB, RR2, 0x01,
        OP_ADDIB, RR3, 0x01,
        OP_ADDIB, RR4, 0x01,
        OP_ADD, RR1, RR2,
        OP_ADD, RR3, RR4,
    };
    setup_vm(&vm, code, S(code));
    P_R_REPEAT(4, process_reg_immb);
    P_R_REPEAT(2, process_reg_reg);
    TEST_ASSERT_EQUAL(vm.regs.r1, 0x02);
    TEST_ASSERT_EQUAL(vm.regs.r2, 0x01);
    TEST_ASSERT_EQUAL(vm.regs.r3, 0x02);
    TEST_ASSERT_EQUAL(vm.regs.r4, 0x01);
}

void test_op_mul(void) {
    u8 code[] = {
        OP_ADDIB, RR1, 0x02,
        OP_ADDIB, RR2, 0x02,
        OP_ADDIB, RR3, 0x02,
        OP_ADDIB, RR4, 0x02,
        OP_MUL, RR1, RR2,
        OP_MUL, RR3, RR4,
    };
    setup_vm(&vm, code, S(code));
    P_R_REPEAT(4, process_reg_immb);
    P_R_REPEAT(2, process_reg_reg);
    TEST_ASSERT_EQUAL(vm.regs.r1, 0x04);
    TEST_ASSERT_EQUAL(vm.regs.r2, 0x02);
    TEST_ASSERT_EQUAL(vm.regs.r3, 0x04);
    TEST_ASSERT_EQUAL(vm.regs.r4, 0x02);
}

void test_op_div(void) {
    u8 code[] = {
        OP_ADDIB, RR1, 0x02,
        OP_ADDIB, RR2, 0x02,
        OP_ADDIB, RR3, 0x02,
        OP_ADDIB, RR4, 0x02,
        OP_DIV, RR1, RR2,
        OP_DIV, RR3, RR4,
    };
    setup_vm(&vm, code, S(code));
    P_R_REPEAT(4, process_reg_immb);
    P_R_REPEAT(2, process_reg_reg);
    TEST_ASSERT_EQUAL(vm.regs.r1, 0x01);
    TEST_ASSERT_EQUAL(vm.regs.r2, 0x02);
    TEST_ASSERT_EQUAL(vm.regs.r3, 0x01);
    TEST_ASSERT_EQUAL(vm.regs.r4, 0x02);
}

void test_op_xor(void) {
    u8 code[] = {
        OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
        OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
        OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
        OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
        OP_XOR, RR1, RR2,
        OP_XOR, RR3, RR4,
    };
    setup_vm(&vm, code, S(code));
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(2, process_reg_reg);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x33333333);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x22222222);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x77777777);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x44444444);
}

void test_op_mov(void) {
    u8 code[] = {
        OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
        OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
        OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
        OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
        OP_MOV, RR1, RR2,
        OP_MOV, RR3, RR4,
    };
    setup_vm(&vm, code, S(code));
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(2, process_reg_reg);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x22222222);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x22222222);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x44444444);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x44444444);
}

void test_op_inc(void) {
    u8 code[] = {
        OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
        OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
        OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
        OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
        OP_INC, RR1,
        OP_INC, RR2,
        OP_INC, RR3,
        OP_INC, RR4,
    };
    setup_vm(&vm, code, S(code));
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(4, process_reg);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x11111111+0x01);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x22222222+0x01);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x33333333+0x01);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x44444444+0x01);
}


void test_op_dec(void) {
    CODE (
        OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
        OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
        OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
        OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
        OP_DEC, RR1,
        OP_DEC, RR2,
        OP_DEC, RR3,
        OP_DEC, RR4,
        );
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(4, process_reg);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x11111111-0x01);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x22222222-0x01);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x33333333-0x01);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x44444444-0x01);
}


