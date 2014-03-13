/* - qnix
 * TODO: Continue implementing syscall
 * TODO: Add more tests for new registers
 * TODO: Implement a sys_call interface
 * TODO: fix values in reg_t vs u32 vs u16 vs u8
 * TODO: registers numbers should be considered as u8 not reg_t
 * XXX: implement TESTS *Check*
 * XXX: maybe convert reg_t to a struct that contains a value, and a string name
 * and maybe other things.. settings ? -reg_to_str(..)
 * =====================================================================
 * Implementation of a simple virtual machine 32bit
 * =====================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/syscall.h>
#include "vm.h"

#define COPY_PARTS8(X, Y, Z) X = (X << 8) | Y[++Z]
#define COPY_PARTS16(X, Y, Z) do { \
			COPY_PARTS8(X, Y, Z); \
			COPY_PARTS8(X, Y, Z); \
			} while(0)
#define COPY_PARTS32(X, Y, Z) do { \
			COPY_PARTS16(X, Y, Z); \
			COPY_PARTS16(X, Y, Z); \
			} while(0)
#define IS_VALID_REG(x) assert(x == RR1 || x == RR2 || \
  x == RR3 || x == RR4 || x == RR5 || x == RR6 || \
  x == RR7 || x == RR8 || x == RPS || x == RPC)

static inst inst_table[100] = {
	/* reg{reg_t}, reg{reg_t} and reg{reg_t} operators */
	{ADD, "add", OP_ADD, inst_add},
	{SUB, "sub", OP_SUB, inst_sub},
	{MUL, "mul", OP_MUL, inst_mul},
	{DIV, "div", OP_DIV, inst_div},
	{XOR, "xor", OP_XOR, inst_xor},
	{MOV, "mov", OP_MOV, inst_mov},
	{INC, "inc", OP_INC, inst_inc},
	{DEC, "dec", OP_DEC, inst_dec},
	{PUSHB, "pushb", OP_PUSHB, NULL},
	{PUSHW, "pushw", OP_PUSHW, NULL},
	{PUSHD, "pushd", OP_PUSHD, NULL},
	{POPB, "popb", OP_POPB, NULL},
	{POPW, "popw", OP_POPW, NULL},
	{POPD, "popd", OP_POPD, NULL},
	/* reg, immu{32,16,8} and immu{32,16,8} operators */
	// immb 8bit
	{ADDIB, "addib", OP_ADDIB, inst_add},
	{SUBIB, "subib", OP_SUBIB, inst_sub},
	{MULIB, "mulib", OP_MULIB, inst_mul},
	{DIVIB, "divib", OP_DIVIB, inst_div},
	{XORIB, "xorib", OP_XORIB, inst_xor},
	{MOVIB, "movib", OP_MOVIB, inst_mov},
	{PUSHIB, "pushib", OP_PUSHIB, NULL},
	// immw 16bit
	{ADDIW, "addiw", OP_ADDIW, inst_add},
	{SUBIW, "subiw", OP_SUBIW, inst_sub},
	{MULIW, "muliw", OP_MULIW, inst_mul},
	{DIVIW, "diviw", OP_DIVIW, inst_div},
	{XORIW, "xoriw", OP_XORIW, inst_xor},
	{MOVIW, "moviw", OP_MOVIW, inst_mov},
	{PUSHIW, "pushiw", OP_PUSHIW, NULL},
	// immd 32bit TODO rename to DW ?
	{ADDID, "addid", OP_ADDID, inst_add},
	{SUBID, "subid", OP_SUBID, inst_sub},
	{MULID, "mulid", OP_MULID, inst_mul},
	{DIVID, "divid", OP_DIVID, inst_div},
	{XORID, "xorid", OP_XORID, inst_xor},
	{MOVID, "movid", OP_MOVID, inst_mov},
	{PUSHID, "pushid", OP_PUSHID, NULL},
	/* others */
	{GSTK, "gstk", OP_GSTK, NULL},
	{PSTK, "pstk", OP_PSTK, NULL},
	{EXIT, "exit", OP_EXIT, NULL},
	{SYSCALL, "sys_call", OP_SYSCALL, NULL}
};

bool init_vm(vm_t * vm, size_t cs, size_t ss)
{
	if (cs < 0 || ss < 0)
		return false;

	memset(&vm->regs, 0, sizeof(vm->regs));
	vm->code_size = cs;
	vm->stack_size = ss;

	// allocate code and stack
	vm->code = (u8 *) malloc(sizeof(u8) * vm->code_size);
	if (vm->code == NULL) {
		perror("malloc");
		return false;
	}
	vm->stack = (u32 *) malloc(sizeof(u32) * vm->stack_size);
	if (vm->stack == NULL) {
		perror("malloc");
		free(vm->code);
		return false;
	}
	// zero code and stack
	memset(vm->code, 0, cs);
	memset(vm->stack, 0, ss);

	return true;
}

bool term_vm(vm_t * vm)
{
	memset(&vm->regs, 0, sizeof(vm->regs));
	vm->code_size = vm->stack_size = 0;
	free(vm->code);
	free(vm->stack);
	return true;
}

enum INST_NAME get_inst_name(u8 opcode)
{
	size_t inst_table_size = sizeof(inst_table) / sizeof(inst_table[0]);
	size_t i;
	for (i = 0; i < inst_table_size; i++) {
		if (inst_table[i].opcode == opcode)
			return inst_table[i].name;
	}
	return INVALID_OPCODE;
}

const char *reg_to_str(reg_t r)
{
	switch (r) {
	case RPC:
		return "pc";
		break;
	case RPS:
		return "ps";
		break;
	case RR1:
		return "r1";
		break;
	case RR2:
		return "r2";
		break;
	case RR3:
		return "r3";
		break;
	case RR4:
		return "r4";
		break;
	case RR5:
		return "r5";
		break;
	case RR6:
		return "r6";
		break;
	case RR7:
		return "r7";
		break;
	case RR8:
		return "r8";
		break;
	default:
		break;
	}

	return "InvalidReg";
}

reg_t *ridx_to_rvm(reg_t r_idx, vm_t * vm)
{
	reg_t *res = NULL;

	switch (r_idx) {
	case RPS:
		res = &vm->regs.ps;
		break;
	case RPC:
		res = &vm->regs.pc;
		break;
	case RR1:
		res = &vm->regs.r1;
		break;
	case RR2:
		res = &vm->regs.r2;
		break;
	case RR3:
		res = &vm->regs.r3;
		break;
	case RR4:
		res = &vm->regs.r4;
		break;
	case RR5:
		res = &vm->regs.r5;
		break;
	case RR6:
		res = &vm->regs.r6;
		break;
	case RR7:
		res = &vm->regs.r7;
		break;
	case RR8:
		res = &vm->regs.r8;
		break;
	default:
		break;
	};

	return res;
}

bool process_code(vm_t * vm, bool execute, bool verbose)
{
	// then nothing to do
	if (!execute && !verbose) {
		return false;
	}

	for (vm->regs.pc = 0; vm->regs.pc < (reg_t) vm->code_size;) {
		u8 opcode = vm->code[vm->regs.pc];
		inst c_inst = get_instruction(opcode);
		SHOW_REGS;
		switch (c_inst.name) {
		case ADD:
			{
				// extract registers
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				IS_VALID_REG(ra_idx);
				IS_VALID_REG(rb_idx);
				// register index to vm register
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					reg_t *rb = ridx_to_rvm(rb_idx, vm);
					assert(ra != NULL);
					assert(rb != NULL);
					// do instruction
					*ra = c_inst.func(*ra, *rb);
				}
				break;
			}
		case ADDIB:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rab;
				COPY_PARTS8(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job     
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u8) rab);
				}
				break;
			}
		case ADDIW:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u16) rab);
				}
				break;
			}
		case ADDID:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job     
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u32) rab);
				}
				break;
			}
		case SUB:
			{
				// extract registers
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				IS_VALID_REG(ra_idx);
				IS_VALID_REG(rb_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					reg_t *rb = ridx_to_rvm(rb_idx, vm);
					assert(ra != NULL);
					assert(rb != NULL);
					*ra = c_inst.func(*ra, *rb);
				}
				break;
			}
		case SUBIB:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rab;
				COPY_PARTS8(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u8) rab);
				}
				break;
			}
		case SUBIW:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u16) rab);
				}
				break;
			}
		case SUBID:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job     
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u32) rab);
				}
				break;
			}
		case MUL:
			{
				// extract registers
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				IS_VALID_REG(ra_idx);
				IS_VALID_REG(rb_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					reg_t *rb = ridx_to_rvm(rb_idx, vm);
					assert(ra != NULL);
					assert(rb != NULL);
					*ra = c_inst.func(*ra, *rb);
				}
				break;
			}
		case MULIB:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rab;
				COPY_PARTS8(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job     
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u8) rab);
				}
				break;
			}
		case MULIW:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u16) rab);
				}
				break;
			}
		case MULID:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u32) rab);
				}
				break;
			}
		case DIV:
			{
				// extract registers
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				IS_VALID_REG(ra_idx);
				IS_VALID_REG(rb_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					reg_t *rb = ridx_to_rvm(rb_idx, vm);
					assert(ra != NULL);
					assert(rb != NULL);
					*ra = c_inst.func(*ra, *rb);
				}
				break;
			}
		case DIVIB:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rab;
				COPY_PARTS8(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u8) rab);
				}
				break;
			}
		case DIVIW:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u16) rab);
				}
				break;
			}
		case DIVID:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u32) rab);
				}
				break;
			}
		case XOR:
			{
				// extract registers
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				IS_VALID_REG(ra_idx);
				IS_VALID_REG(rb_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					reg_t *rb = ridx_to_rvm(rb_idx, vm);
					assert(ra != NULL);
					assert(rb != NULL);
					*ra = c_inst.func(*ra, *rb);
				}
				break;
			}
		case XORIB:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rab;
				COPY_PARTS8(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job     
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u8) rab);
				}
				break;
			}
		case XORIW:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u16) rab);
				}
				break;
			}
		case XORID:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u32) rab);
				}
				break;
			}
		case MOV:
			{
				// extract registers
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				IS_VALID_REG(ra_idx);
				IS_VALID_REG(rb_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					reg_t *rb = ridx_to_rvm(rb_idx, vm);
					assert(ra != NULL);
					assert(rb != NULL);
					*ra = c_inst.func(*ra, *rb);
				}
				break;
			}
		case MOVIB:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u8 rab;
				COPY_PARTS8(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u8) rab);
				}
				break;
			}
		case MOVIW:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u16) rab);
				}
				break;
			}
		case MOVID:
			{
				// extract register and byte
				u8 ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %s %.8x\n",
					       c_inst.sname, reg_to_str(ra_idx),
					       rab);
				IS_VALID_REG(ra_idx);
				// does job
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, (u32) rab);
				}
				break;
			}
		case INC:
			{
				// extract registers
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, 0);
				}
				break;
			}
		case DEC:
			{
				// extract registers
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					assert(ra != NULL);
					*ra = c_inst.func(*ra, 0);
				}
				break;
			}
		case PUSHB:
			{
				// extract registers
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->stack[vm->regs.ps++] = (u8) * ra;
				}
				break;
			}
		case PUSHW:
			{
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->stack[vm->regs.ps++] = (u16) * ra;
				}
				break;
			}
		case PUSHD:
			{
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->stack[vm->regs.ps++] = (u32) * ra;
				}
				break;
			}
		case POPB:
			{
				assert(vm->regs.ps > 0);
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->regs.ps--;
					*ra = (u8) vm->stack[vm->regs.ps];
					vm->stack[vm->regs.ps] = 0x00000000;
				}
				break;
			}
		case POPW:
			{
				assert(vm->regs.ps >= 0);
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->regs.ps--;
					*ra = (u16) vm->stack[vm->regs.ps];
					vm->stack[vm->regs.ps] = 0x00000000;
				}
				break;
			}
		case POPD:
			{
				assert(vm->regs.ps > 0);
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->regs.ps--;
					*ra = (u32) vm->stack[vm->regs.ps];
				}
				break;
			}
		case PUSHIB:
			{
				reg_t ra = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %.8x\n", c_inst.sname,
					       ra);
				if (execute) {
					vm->stack[vm->regs.ps++] = (u8) ra;
				}
				break;
			}
		case PUSHIW:
			{
				u16 ra;
				COPY_PARTS16(ra, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %.8x\n", c_inst.sname,
					       ra);
				if (execute) {
					vm->stack[vm->regs.ps++] = (u16) ra;
				}
				break;
			}
		case PUSHID:
			{
				u32 ra;
				COPY_PARTS32(ra, vm->code, vm->regs.pc);
				if (verbose)
					printf(">>>>> %s %.8x\n", c_inst.sname,
					       ra);
				if (execute) {
					vm->stack[vm->regs.ps++] = (u32) ra;
				}
				break;
			}
		case GSTK:
			{
				assert(vm->regs.ps > 0);
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					*ra = &vm->stack[vm->regs.ps - 1];
				}
				break;
			}
		case PSTK:
			{
				u8 ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf(">>>>> %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				IS_VALID_REG(ra_idx);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->stack[vm->regs.ps++] =
					    *(u32 *) (*ra);
				}
				break;
			}
		case EXIT:
			{
				if (verbose)
					printf("%s\n", c_inst.sname);
				if (execute) {
					return true;
				}
				break;
			}
		case SYSCALL:
			{
				reg_t scall = vm->regs.r1;
				reg_t nargs = get_syscall_nargs(scall);
				if (nargs == -1) {
					printf("Invalid system call\n");
					return false;
				}
				printf("%s_%.2x(%d arguments)\n",
				       c_inst.sname, scall, nargs);
				// do a syscall depending on nargs
				// number
				if (nargs == 0) {
					vm->regs.r1 = syscall(scall);
					printf("0 returned %d\n", vm->regs.r1);
				} else if (nargs == 1) {
					vm->regs.r1 = syscall(scall,
							      vm->regs.r2);
					printf("1 returned %d\n", vm->regs.r1);
				} else if (nargs == 2) {
					vm->regs.r1 = syscall(scall,
							      vm->regs.r2,
							      vm->regs.r3);
					printf("2 returned %d\n", vm->regs.r1);
				} else if (nargs == 3) {
					vm->regs.r1 = syscall(scall,
							      vm->regs.r2,
							      vm->regs.r3,
							      vm->regs.r4);
					printf("3 returned %d\n", vm->regs.r1);
				} else if (nargs == 4) {
					vm->regs.r1 = syscall(scall,
							      vm->regs.r2,
							      vm->regs.r3,
							      vm->regs.r4,
							      vm->regs.r5);
					printf("4 returned %d\n", vm->regs.r1);
				} else if (nargs == 5) {
					vm->regs.r1 = syscall(scall,
							      vm->regs.r2,
							      vm->regs.r3,
							      vm->regs.r4,
							      vm->regs.r5,
							      vm->regs.r6);
					printf("5 returned %d\n", vm->regs.r1);
				} else if (nargs == 6) {
					vm->regs.r1 = syscall(scall,
							      vm->regs.r2,
							      vm->regs.r3,
							      vm->regs.r4,
							      vm->regs.r5,
							      vm->regs.r6,
							      vm->regs.r7);
					printf("6 returned %d\n", vm->regs.r1);
				} else if (nargs == 7) {
					vm->regs.r1 = syscall(scall,
							      vm->regs.r2,
							      vm->regs.r3,
							      vm->regs.r4,
							      vm->regs.r5,
							      vm->regs.r6,
							      vm->regs.r7,
							      vm->regs.r8);
					printf("7 returned %d\n", vm->regs.r1);
				}
				// int n = get_syscall_regn(r1);
				break;
			}
		case INVALID_OPCODE:
			{
				// XXX incase of an invalid opcode shall we
				// -ignore and continue ?
				if (verbose)
					printf(">>>>> %s %.8x\n", c_inst.sname,
					       c_inst.opcode);
				if (execute) {
					return false;
				}
				break;
			}
		default:
			printf("default\n");
			return false;
		};
		vm->regs.pc++;
	}

	return true;
}

inst get_instruction(u8 opcode)
{
	size_t inst_table_size = sizeof(inst_table) / sizeof(inst_table[0]);
	size_t i;
	for (i = 0; i < inst_table_size; i++) {
		if (inst_table[i].opcode == opcode)
			return inst_table[i];
	}
	return (inst) {
	INVALID_OPCODE, "InvalidOp", opcode, NULL};	// TODO INVALID INSTRUCTION
}

reg_t inst_add(reg_t a, reg_t b)
{
	a += b;
	return a;
}

reg_t inst_sub(reg_t a, reg_t b)
{
	a -= b;
	return a;
}

reg_t inst_mul(reg_t a, reg_t b)
{
	a *= b;
	return a;
}

reg_t inst_div(reg_t a, reg_t b)
{
	assert(b != 0);		// TODO better handling raise SIGFPE-like
	a /= b;
	return a;
}

reg_t inst_xor(reg_t a, reg_t b)
{
	a ^= b;
	return a;
}

reg_t inst_mov(reg_t a, reg_t b)
{
	a = b;
	return a;
}

reg_t inst_inc(reg_t a, reg_t unused)
{
	a++;
	return a;
}

reg_t inst_dec(reg_t a, reg_t unused)
{
	a--;
	return a;
}

reg_t get_syscall_nargs(reg_t scall)
{
	switch (scall) {
	case SYS_exit:
		return 1;
		break;
	case SYS_fork:
		return 1;
		break;
	case SYS_read:
		return 3;
		break;
	case SYS_write:
		return 3;
		break;
	case SYS_open:
		return 3;
		break;
	case SYS_close:
		return 1;
		break;
	case SYS_creat:
		return 2;
		break;
	case SYS_link:
		return 2;
		break;
	case SYS_unlink:
		return 1;
		break;
		//.....
	}
	return -1;
}
