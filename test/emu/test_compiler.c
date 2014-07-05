#include "unity.h"
#include "compiler.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_module_generator_needs_to_be_implemented(void) {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_compile(void) {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_assemble(void) {
    TEST_IGNORE_MESSAGE("Implement me!");
}
void test_translate_line(void) {
    // add more tests
    char test_reg_reg[] =
    "mov r1, r2;\n"
    "sub r3, r4;\n"
    "add r1, r3;\n"
    ;
    char test_reg_immb[] =
    "movib r1, 0x11;\n"
    ;
    char test_reg_immw[] =
    "moviw r1, 0x2233;\n"
    ;
    char test_reg_immd[] =
    "movid r3, 0xd34dc0d3;\n"
    ;

    TEST_ASSERT_EQUAL_STRING(translate_line(test_reg_reg), "a60203a20405a10204");
    TEST_ASSERT_EQUAL_STRING(translate_line(test_reg_immb), "b60211");
    TEST_ASSERT_EQUAL_STRING(translate_line(test_reg_immw), "d6022233");
    TEST_ASSERT_EQUAL_STRING(translate_line(test_reg_immd), "c604d34dc0d3");
}

void test_read_file_content() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_reg_str_to_byte(void) {
    TEST_ASSERT_EQUAL_HEX(reg_str_to_byte("R1"), RR1);
    TEST_ASSERT_EQUAL_HEX(reg_str_to_byte("R2"), RR2);
    TEST_ASSERT_EQUAL_HEX(reg_str_to_byte("R3"), RR3);
    TEST_ASSERT_EQUAL_HEX(reg_str_to_byte("r1"), RR1);
    TEST_ASSERT_EQUAL_HEX(reg_str_to_byte("r2"), RR2);
    TEST_ASSERT_EQUAL_HEX(reg_str_to_byte("r3"), RR3);
}
void test_immb_str_to_byte(void) {
  TEST_ASSERT_EQUAL_HEX(immb_str_to_byte("0x41"), 0x41);
  TEST_ASSERT_EQUAL_HEX(immb_str_to_byte("ff"), 0xff);
  TEST_ASSERT_EQUAL_HEX(immb_str_to_byte("0x11"), 0x11);
  TEST_ASSERT_EQUAL_HEX(immb_str_to_byte("0x00"), 0x00);
  TEST_ASSERT_EQUAL_HEX(immb_str_to_byte("dd"), 0xdd);
  TEST_ASSERT_EQUAL_HEX(immb_str_to_byte("ffee"), 0xee);
}

void test_immw_str_to_byte(void) {
  TEST_ASSERT_EQUAL_HEX(immw_str_to_byte("0x1122"), 0x1122);
  TEST_ASSERT_EQUAL_HEX(immw_str_to_byte("1122"), 0x1122);
  TEST_ASSERT_EQUAL_HEX(immw_str_to_byte("11223344"), 0x3344);
}

void test_immd_str_to_byte(void) {
  TEST_ASSERT_EQUAL_HEX(immd_str_to_byte("11223344"), 0x11223344);
  TEST_ASSERT_EQUAL_HEX(immd_str_to_byte("0x11223344"), 0x11223344);
  TEST_ASSERT_EQUAL_HEX(immd_str_to_byte("0x00003344"), 0x00003344);
  TEST_ASSERT_EQUAL_HEX(immd_str_to_byte("00003344"), 0x00003344);
}
