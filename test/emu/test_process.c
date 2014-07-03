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

void test_op_pushb(void) {
    CODE (
      OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
         OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
         OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
         OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
         OP_PUSHB, RR1,
         OP_PUSHB, RR2,
         OP_PUSHB, RR3,
         OP_PUSHB, RR4,
         );
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(4, process_pushb);
    TEST_ASSERT_EQUAL_HEX(vm.stack[0], 0x11);
    TEST_ASSERT_EQUAL_HEX(vm.stack[1], 0x22);
    TEST_ASSERT_EQUAL_HEX(vm.stack[2], 0x33);
    TEST_ASSERT_EQUAL_HEX(vm.stack[3], 0x44);
}

void test_op_pushw(void) {
    CODE (
      OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
         OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
         OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
         OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
         OP_PUSHW, RR1,
         OP_PUSHW, RR2,
         OP_PUSHW, RR3,
         OP_PUSHW, RR4,
         );
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(4, process_pushw);
    TEST_ASSERT_EQUAL_HEX(vm.stack[0], 0x1111);
    TEST_ASSERT_EQUAL_HEX(vm.stack[1], 0x2222);
    TEST_ASSERT_EQUAL_HEX(vm.stack[2], 0x3333);
    TEST_ASSERT_EQUAL_HEX(vm.stack[3], 0x4444);
}

void test_op_pushd(void) {
    CODE (
      OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
         OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
         OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
         OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
         OP_PUSHD, RR1,
         OP_PUSHD, RR2,
         OP_PUSHD, RR3,
         OP_PUSHD, RR4,
         );
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(4, process_pushd);
    TEST_ASSERT_EQUAL_HEX(vm.stack[0], 0x11111111);
    TEST_ASSERT_EQUAL_HEX(vm.stack[1], 0x22222222);
    TEST_ASSERT_EQUAL_HEX(vm.stack[2], 0x33333333);
    TEST_ASSERT_EQUAL_HEX(vm.stack[3], 0x44444444);
}


void test_op_popb(void) {
    CODE (
      OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
         OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
         OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
         OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
         OP_PUSHD, RR1,
         OP_PUSHD, RR2,
         OP_PUSHD, RR3,
         OP_PUSHD, RR4,
         OP_POPB, RR1,
         OP_POPB, RR2,
         OP_POPB, RR3,
         OP_POPB, RR4,
         );
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(4, process_pushd);
    P_R_REPEAT(4, process_popb);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x44);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x33);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x22);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x11);
}

void test_op_popw(void) {
    CODE (
      OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
         OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
         OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
         OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
         OP_PUSHD, RR1,
         OP_PUSHD, RR2,
         OP_PUSHD, RR3,
         OP_PUSHD, RR4,
         OP_POPW, RR1,
         OP_POPW, RR2,
         OP_POPW, RR3,
         OP_POPW, RR4,
         );
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(4, process_pushd);
    P_R_REPEAT(4, process_popw);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x4444);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x3333);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x2222);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x1111);
}

void test_op_popd(void) {
    CODE (
      OP_ADDID, RR1, 0x11, 0x11, 0x11, 0x11,
         OP_ADDID, RR2, 0x22, 0x22, 0x22, 0x22,
         OP_ADDID, RR3, 0x33, 0x33, 0x33, 0x33,
         OP_ADDID, RR4, 0x44, 0x44, 0x44, 0x44,
         OP_PUSHD, RR1,
         OP_PUSHD, RR2,
         OP_PUSHD, RR3,
         OP_PUSHD, RR4,
         OP_POPD, RR1,
         OP_POPD, RR2,
         OP_POPD, RR3,
         OP_POPD, RR4,
         );
    P_R_REPEAT(4, process_reg_immd);
    P_R_REPEAT(4, process_pushd);
    P_R_REPEAT(4, process_popd);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x44444444);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x33333333);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x22222222);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x11111111);
}

void test_op_addib(void) {
    CODE (
      OP_ADDIB, RR1, 0x11,
         OP_ADDIB, RR2, 0x22,
         OP_ADDIB, RR3, 0x33,
         OP_ADDIB, RR4, 0x44,
         );
    P_R_REPEAT(4, process_reg_immb);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x11);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x22);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x33);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x44);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r5, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r6, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r7, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r8, 0x00);
}

void test_op_subib(void) {
    CODE (
      OP_SUBIB, RR1, 0x11,
         OP_SUBIB, RR2, 0x11,
         OP_SUBIB, RR3, 0x11,
         OP_SUBIB, RR4, 0x11,
         );
    P_R_REPEAT(4, process_reg_immb);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0xffffffef);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0xffffffef);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0xffffffef);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0xffffffef);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r5, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r6, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r7, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r8, 0x00);
}

void test_op_mulib(void) {
    CODE (
      OP_XOR, RR1, RR1,
         OP_XOR, RR2, RR2,
         OP_XOR, RR3, RR3,
         OP_XOR, RR4, RR4,
         OP_INC, RR1,
         OP_INC, RR2,
         OP_INC, RR3,
         OP_INC, RR4,
         OP_MULIB, RR1, 0x11,
         OP_MULIB, RR2, 0x11,
         OP_MULIB, RR3, 0x11,
         OP_MULIB, RR4, 0x11,
         );
    P_R_REPEAT(4, process_reg_reg);
    P_R_REPEAT(4, process_reg);
    P_R_REPEAT(4, process_reg_immb);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x11);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x11);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x11);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x11);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r5, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r6, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r7, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r8, 0x00);
}

void test_op_divib(void) {
    CODE (
      OP_MOVIB, RR1, 0x18,
         OP_MOVIB, RR2, 0x18,
         OP_MOVIB, RR3, 0x18,
         OP_MOVIB, RR4, 0x18,
         OP_DIVIB, RR1, 0x04,
         OP_DIVIB, RR2, 0x04,
         OP_DIVIB, RR3, 0x04,
         OP_DIVIB, RR4, 0x04,
         );
    P_R_REPEAT(8, process_reg_immb);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x06);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x06);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x06);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x06);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r5, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r6, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r7, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r8, 0x00);
}

void test_op_xorib(void) {
    CODE (
      OP_MOVIB, RR1, 0x18,
         OP_MOVIB, RR2, 0x18,
         OP_MOVIB, RR3, 0x28,
         OP_MOVIB, RR4, 0x28,
         OP_XORIB, RR1, RR2, // rr1 = 0x18 ^ 0x18;
         OP_XORIB, RR3, RR4, // rr3 = 0x28 ^ 0x28;
         OP_XORIB, RR2, RR4, // rr2 = 0x18 ^ 0x28;
         OP_XORIB, RR4, RR4, // rr4 = 0x00;
         );
    P_R_REPEAT(4, process_reg_immb);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x18);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x18);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x28);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x28);
    P_R_REPEAT(4, process_reg_immb);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x00);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x00);
}

void test_op_pushib(void) {
    CODE (
      OP_PUSHIB, 0x11,
         OP_PUSHIB, 0x22,
         OP_PUSHIB, 0x33,
         OP_PUSHIB, 0x44,
         OP_POPB, RR1,
         OP_POPB, RR2,
         OP_POPB, RR3,
         OP_POPB, RR4,
         );

    P_R_REPEAT(4, process_pushib);
    TEST_ASSERT_EQUAL_HEX(vm.regs.ps, 0x04);
    P_R_REPEAT(4, process_popb);
    TEST_ASSERT_EQUAL_HEX(vm.regs.ps, 0x00);

    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x44);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x33);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x22);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r4, 0x11);
    TEST_ASSERT_EQUAL_HEX(vm.regs.ps, 0x00);
}

void test_op_addiw(void) {
    CODE (
      OP_ADDIW, RR1, 0x11, 0x11,
         OP_ADDIW, RR2, 0x22, 0x22,
         OP_ADDIW, RR3, 0x33, 0x33,  
         );

    P_R_REPEAT(3, process_reg_immw);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, 0x1111);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, 0x2222);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, 0x3333);
}

void test_op_subiw(void) {
    CODE (
      OP_SUBIW, RR1, 0x11, 0x11,
         OP_SUBIW, RR2, 0x22, 0x22,
         OP_SUBIW, RR3, 0x33, 0x33,  
         );

    P_R_REPEAT(3, process_reg_immw);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r1, ~0x1111+1);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r2, ~0x2222+1);
    TEST_ASSERT_EQUAL_HEX(vm.regs.r3, ~0x3333+1);
}
