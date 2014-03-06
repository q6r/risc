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
    ADD, SUB, MUL, DIV, XOR, MOV, INC, DEC,      /* reg(reg_t) */
	ADDIB, SUBIB, MULIB, DIVIB, XORIB, MOVIB,    /* immb(8) */
	ADDIW, SUBIW, MULIW, DIVIW, XORIW, MOVIW,    /* immw(16) */
	ADDID, SUBID, MULID, DIVID, XORID, MOVID,    /* immd(32) */
	PUSHB, PUSHW, PUSHD, POPB, POPW, POPD,       /* reg(reg_t) */
// TODO
//	PUSHIB, PUSHIW, PUSHID, POPIB, POPIW, POPID, /* immu(32,16,8) */
    EXIT, INVALID_OPCODE };

typedef struct {
	enum INST_NAME name;
	u8 opcode;
	reg_t (*func)(reg_t a, reg_t b);
} inst;

typedef struct {
	reg_t pc;
	reg_t ps;
	reg_t r1,r2,r3,r4;
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

bool init_vm(vm_t *vm, size_t cs, size_t ss);
bool term_vm(vm_t *vm);
bool process_code(vm_t *vm);

enum INST_NAME get_inst_name(u8 opcode);
inst get_instruction(u8 opcode);
reg_t *ridx_to_rvm(reg_t r_idx, vm_t *vm);

static inst inst_table[100] =
	{
		// reg
		{ADD, 0xa1, inst_add},
		{SUB, 0xa2, inst_sub},
		{MUL, 0xa3, inst_mul},
		{DIV, 0xa4, inst_div},
		{XOR, 0xa5, inst_xor},
		{MOV, 0xa6, inst_mov},
		// reg only
		{INC, 0xa7, inst_inc},
		{DEC, 0xa8, inst_dec},
		// immb 8bit
		{ADDIB, 0xb1, inst_add},
		{SUBIB, 0xb2, inst_sub},
		{MULIB, 0xb3, inst_mul},
		{DIVIB, 0xb4, inst_div},
		{XORIB, 0xb5, inst_xor},
		{MOVIB, 0xb6, inst_mov},
		// immw 16bit
		{ADDIW, 0xd1, inst_add},
		{SUBIW, 0xd2, inst_sub},
		{MULIW, 0xd3, inst_mul},
		{DIVIW, 0xd4, inst_div},
		{XORIW, 0xd5, inst_xor},
		{MOVIW, 0xb6, inst_mov},
	    // immd 32bit TODO rename to DW ?
		{ADDID, 0xc1, inst_add},
		{SUBID, 0xc2, inst_sub},
		{MULID, 0xc3, inst_mul},
		{DIVID, 0xc4, inst_div},
		{XORID, 0xc5, inst_xor},
		{MOVID, 0xc6, inst_mov},
		// reg only
		{PUSHB, 0xa9, NULL},
		{PUSHW, 0xaa, NULL},
		{PUSHD, 0xab, NULL},
		{POPB,  0xac, NULL},
		{POPW,  0xad, NULL},
		{POPD,  0xae, NULL},
		{EXIT,  0xaf, NULL}
	};

#endif
