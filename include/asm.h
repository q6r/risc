#ifndef __ASM_H_
#define __ASM_H_

#include <stdbool.h>
#include "emu.h"

#define INVALID_REG 0xff

typedef struct {
	u8 opcode;
	u8 reg;
	union Val {
		u8 v8;
		u16 v16;
		u32 v32;
	} val;
} binary_code_t;

typedef struct {
	char name[256];
	u8 opcode;
	u32 flag;		// T_REG, T_IMMU, T_NOTH.
	u32 type;		// T_{8,16,32}
} opcode_bundle_t;

typedef struct {
	opcode_bundle_t ob;
} translate_t;

#define T_REGISTER 1
#define T_IMMU 2
#define T_NOTHING 4
#define T_8 8
#define T_16 16
#define T_32 32

translate_t translation_table[] = {
	/* reg{reg_t}, reg{reg_t} and reg{reg_t} operators */
	{"add", OP_ADD, T_REGISTER},
	{"sub", OP_SUB, T_REGISTER},
	{"mul", OP_MUL, T_REGISTER},
	{"div", OP_DIV, T_REGISTER},
	{"xor", OP_XOR, T_REGISTER},
	{"mov", OP_MOV, T_REGISTER},
	{"inc", OP_INC, T_REGISTER},
	{"dec", OP_DEC, T_REGISTER},
	{"pushb", OP_PUSHB, T_REGISTER | T_8},
	{"pushw", OP_PUSHW, T_REGISTER | T_16},
	{"pushd", OP_PUSHD, T_REGISTER | T_32},
	{"popb", OP_POPB, T_REGISTER | T_8},
	{"popw", OP_POPW, T_REGISTER | T_16},
	{"popd", OP_POPD, T_REGISTER | T_32},
	/* reg, immu{32,16,8} and immu{32,16,8} operators */
	// immb 8bit
	{"addib", OP_ADDIB, T_REGISTER | T_IMMU | T_8},
	{"subib", OP_SUBIB, T_REGISTER | T_IMMU | T_8},
	{"mulib", OP_MULIB, T_REGISTER | T_IMMU | T_8},
	{"divib", OP_DIVIB, T_REGISTER | T_IMMU | T_8},
	{"xorib", OP_XORIB, T_REGISTER | T_IMMU | T_8},
	{"movib", OP_MOVIB, T_REGISTER | T_IMMU | T_8},
	{"pushib", OP_PUSHIB, T_IMMU | T_8},
	// immw 16bit
	{"addiw", OP_ADDIW, T_REGISTER | T_IMMU | T_16},
	{"subiw", OP_SUBIW, T_REGISTER | T_IMMU | T_16},
	{"muliw", OP_MULIW, T_REGISTER | T_IMMU | T_16},
	{"diviw", OP_DIVIW, T_REGISTER | T_IMMU | T_16},
	{"xoriw", OP_XORIW, T_REGISTER | T_IMMU | T_16},
	{"moviw", OP_MOVIW, T_REGISTER | T_IMMU | T_16},
	{"pushiw", OP_PUSHIW, T_IMMU | T_16},
	// immd 32bit TODO rename to DW ?
	{"addid", OP_ADDID, T_REGISTER | T_IMMU | T_32},
	{"subid", OP_SUBID, T_REGISTER | T_IMMU | T_32},
	{"mulid", OP_MULID, T_REGISTER | T_IMMU | T_32},
	{"divid", OP_DIVID, T_REGISTER | T_IMMU | T_32},
	{"xorid", OP_XORID, T_REGISTER | T_IMMU | T_32},
	{"movid", OP_MOVID, T_REGISTER | T_IMMU | T_32},
	{"pushid", OP_PUSHID, T_IMMU | T_32},
	/* others */
	{"exit", OP_EXIT, T_NOTHING},
	{"sys_call", OP_SYSCALL, T_NOTHING}
};

bool convert_to_binarycode(binary_code_t * res, char *line);
bool get_opcode_bundle(opcode_bundle_t * res, char *opcode);
u8 regstr_to_regbyte(char *reg);

#endif
