/* - qnix
 * =====================================================================================
 * Implementation of a simple virtual machine 32bit
 * =====================================================================================
 */

#ifndef __VM_H__
#define __VM_H__

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#define SHOW_REGS \
printf("pc = %.8x\n", vm->regs.pc);\
printf("ps = %.8x\n", vm->regs.ps);\
printf("r1 = %.8x\n", vm->regs.r1);\
printf("r2 = %.8x\n", vm->regs.r2);\
printf("r3 = %.8x\n", vm->regs.r3);\
printf("r4 = %.8x\n", vm->regs.r4);\

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef u32 reg_t;

enum INST_NAME {
	ADD, SUB, MUL, DIV, XOR, MOV, INC, DEC,	/* reg(reg_t) */
	ADDIB, SUBIB, MULIB, DIVIB, XORIB, MOVIB,	/* immb(8) */
	ADDIW, SUBIW, MULIW, DIVIW, XORIW, MOVIW,	/* immw(16) */
	ADDID, SUBID, MULID, DIVID, XORID, MOVID,	/* immd(32) */
	PUSHB, PUSHW, PUSHD, POPB, POPW, POPD,	/* reg(reg_t) */
// TODO
//      PUSHIB, PUSHIW, PUSHID, POPIB, POPIW, POPID, /* immu(32,16,8) */
	EXIT, INVALID_OPCODE
};

typedef struct {
	enum INST_NAME name;
	char sname[32];
	u8 opcode;
	 reg_t(*func) (reg_t a, reg_t b);
} inst;

typedef struct {
	reg_t pc;		/*  0x04 */
	reg_t ps;		/*  0x05 */
	reg_t r1,		/*  0x00 */
	 r2,			/*  0x01 */
	 r3,			/*  0x02 */
	 r4;			/*  0x03 */
} regs_t;

typedef struct {
	regs_t regs;
	size_t code_size;
	size_t stack_size;
	u8 *code;
	u8 *stack;
} vm_t;

// TODO change all rets to reg_t
// since reg_t is the reciever
// casting are in the inst_*()
// reg
reg_t inst_add(reg_t a, reg_t b);
reg_t inst_sub(reg_t a, reg_t b);
reg_t inst_mul(reg_t a, reg_t b);
reg_t inst_div(reg_t a, reg_t b);
reg_t inst_xor(reg_t a, reg_t b);
reg_t inst_mov(reg_t a, reg_t b);
// reg only
reg_t inst_inc(reg_t a, reg_t unused);
reg_t inst_dec(reg_t a, reg_t unused);

bool init_vm(vm_t * vm, size_t cs, size_t ss);
bool term_vm(vm_t * vm);
bool process_code(vm_t * vm);

enum INST_NAME get_inst_name(u8 opcode);
inst get_instruction(u8 opcode);
reg_t *ridx_to_rvm(reg_t r_idx, vm_t * vm);

const char *reg_to_str(reg_t r);

static inst inst_table[100] = {
	// reg
	{ADD, "add", 0xa1, inst_add},
	{SUB, "sub", 0xa2, inst_sub},
	{MUL, "mul", 0xa3, inst_mul},
	{DIV, "div", 0xa4, inst_div},
	{XOR, "xor", 0xa5, inst_xor},
	{MOV, "mov", 0xa6, inst_mov},
	// reg only
	{INC, "inc", 0xa7, inst_inc},
	{DEC, "dec", 0xa8, inst_dec},
	// immb 8bit
	{ADDIB, "addib", 0xb1, inst_add},
	{SUBIB, "subib", 0xb2, inst_sub},
	{MULIB, "mulib", 0xb3, inst_mul},
	{DIVIB, "divib", 0xb4, inst_div},
	{XORIB, "xorib", 0xb5, inst_xor},
	{MOVIB, "movib", 0xb6, inst_mov},
	// immw 16bit
	{ADDIW, "addiw", 0xd1, inst_add},
	{SUBIW, "subiw", 0xd2, inst_sub},
	{MULIW, "muliw", 0xd3, inst_mul},
	{DIVIW, "diviw", 0xd4, inst_div},
	{XORIW, "xoriw", 0xd5, inst_xor},
	{MOVIW, "moviw", 0xb6, inst_mov},
	// immd 32bit TODO rename to DW ?
	{ADDID, "addid", 0xc1, inst_add},
	{SUBID, "subid", 0xc2, inst_sub},
	{MULID, "mulid", 0xc3, inst_mul},
	{DIVID, "divid", 0xc4, inst_div},
	{XORID, "xorid", 0xc5, inst_xor},
	{MOVID, "movid", 0xc6, inst_mov},
	// reg only
	{PUSHB, "pushb", 0xa9, NULL},
	{PUSHW, "pushw", 0xaa, NULL},
	{PUSHD, "pushd", 0xab, NULL},
	{POPB, "popb", 0xac, NULL},
	{POPW, "popw", 0xad, NULL},
	{POPD, "popd", 0xae, NULL},
	{EXIT, "exit", 0xaf, NULL}
};

#endif
