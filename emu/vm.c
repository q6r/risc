/* - qnix
 * TODO: user COPY_PARTS8 in 8bit operators
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
#include "emu.h"

#define COPY_PARTS8(X, Y, Z) X = (X << 8) | Y[++Z]
#define COPY_PARTS16(X, Y, Z) do { \
			COPY_PARTS8(X, Y, Z); \
			COPY_PARTS8(X, Y, Z); \
			} while(0)
#define COPY_PARTS32(X, Y, Z) do { \
			COPY_PARTS16(X, Y, Z); \
			COPY_PARTS16(X, Y, Z); \
			} while(0)

static inst inst_table[100] = {
	/* reg{reg_t}, reg{reg_t} and reg{reg_t} operators */
	{ADD, "add", 0xa1, inst_add},
	{SUB, "sub", 0xa2, inst_sub},
	{MUL, "mul", 0xa3, inst_mul},
	{DIV, "div", 0xa4, inst_div},
	{XOR, "xor", 0xa5, inst_xor},
	{MOV, "mov", 0xa6, inst_mov},
	{INC, "inc", 0xa7, inst_inc},
	{DEC, "dec", 0xa8, inst_dec},
	{PUSHB, "pushb", 0xa9, NULL},
	{PUSHW, "pushw", 0xaa, NULL},
	{PUSHD, "pushd", 0xab, NULL},
	{POPB, "popb", 0xac, NULL},
	{POPW, "popw", 0xad, NULL},
	{POPD, "popd", 0xae, NULL},
	/* reg, immu{32,16,8} and immu{32,16,8} operators */
	// immb 8bit
	{ADDIB, "addib", 0xb1, inst_add},
	{SUBIB, "subib", 0xb2, inst_sub},
	{MULIB, "mulib", 0xb3, inst_mul},
	{DIVIB, "divib", 0xb4, inst_div},
	{XORIB, "xorib", 0xb5, inst_xor},
	{MOVIB, "movib", 0xb6, inst_mov},
	{PUSHIB, "pushib", 0xd9, NULL},
	// immw 16bit
	{ADDIW, "addiw", 0xd1, inst_add},
	{SUBIW, "subiw", 0xd2, inst_sub},
	{MULIW, "muliw", 0xd3, inst_mul},
	{DIVIW, "diviw", 0xd4, inst_div},
	{XORIW, "xoriw", 0xd5, inst_xor},
	{MOVIW, "moviw", 0xd6, inst_mov},
	{PUSHIW, "pushiw", 0xda, NULL},
	// immd 32bit TODO rename to DW ?
	{ADDID, "addid", 0xc1, inst_add},
	{SUBID, "subid", 0xc2, inst_sub},
	{MULID, "mulid", 0xc3, inst_mul},
	{DIVID, "divid", 0xc4, inst_div},
	{XORID, "xorid", 0xc5, inst_xor},
	{MOVID, "movid", 0xc6, inst_mov},
	{PUSHID, "pushid", 0xdb, NULL},
	/* others */
	{EXIT, "exit", 0xaf, NULL}
};



bool init_vm(vm_t * vm, size_t cs, size_t ss)
{
	assert(cs > 0);
	assert(ss > 0);

	vm->regs.pc = 0x00000000;
	vm->regs.ps = 0x00000000;
	vm->regs.r1 = 0x00000000;
	vm->regs.r2 = 0x00000000;
	vm->regs.r3 = 0x00000000;
	vm->regs.r4 = 0x00000000;
	vm->code_size = cs;
	vm->stack_size = ss;

	// allocate code and stack
	vm->code = (u8 *) malloc(sizeof(u8) * vm->code_size);
	assert(vm->code != NULL);
	vm->stack = (u8 *) malloc(sizeof(int8_t) * vm->stack_size);
	assert(vm->stack != NULL);
	// zero code and stack
	memset(vm->code, 0x00, cs);
	memset(vm->stack, 0x00, ss);

	return true;
}

bool term_vm(vm_t * vm)
{
	vm->regs.pc = 0x00000000;
	vm->regs.ps = 0x00000000;
	vm->regs.r1 = 0x00000000;
	vm->regs.r2 = 0x00000000;
	vm->regs.r3 = 0x00000000;
	vm->regs.r4 = 0x00000000;
	vm->code_size = 0x00;
	vm->stack_size = 0x00;
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
	case 0x00:
		return "r1";
		break;
	case 0x01:
		return "r2";
		break;
	case 0x02:
		return "r3";
		break;
	case 0x03:
		return "r4";
		break;
	case 0x04:
		return "pc";
		break;
	case 0x05:
		return "ps";
		break;
	}
	return "InvalidReg";
}

reg_t *ridx_to_rvm(reg_t r_idx, vm_t * vm)
{
	reg_t *res = NULL;

	switch (r_idx) {
	case 0x00:
		res = &vm->regs.r1;
		break;
	case 0x01:
		res = &vm->regs.r2;
		break;
	case 0x02:
		res = &vm->regs.r3;
		break;
	case 0x03:
		res = &vm->regs.r4;
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %s\n", c_inst.sname,
					       reg_to_str(ra_idx),
					       reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab;
				COPY_PARTS16(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab;
				COPY_PARTS32(rab, vm->code, vm->regs.pc);
				if (verbose)
					printf("%s %s %.8x\n", c_inst.sname,
					       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->stack[vm->regs.ps++] = (u8) * ra;
				}
				break;
			}
		case PUSHW:
			{
				reg_t ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->stack[vm->regs.ps++] = (u16) * ra;
				}
				break;
			}
		case PUSHD:
			{
				reg_t ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				if (execute) {
					reg_t *ra = ridx_to_rvm(ra_idx, vm);
					vm->stack[vm->regs.ps++] = (u32) * ra;
				}
				break;
			}
		case POPB:
			{
				assert(vm->regs.ps > 0);
				reg_t ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
				reg_t ra_idx = vm->code[++vm->regs.pc];
				if (verbose)
					printf("%s %s\n", c_inst.sname,
					       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
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
					printf("%s %.8x\n", c_inst.sname, ra);
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
					printf("%s %.8x\n", c_inst.sname, ra);
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
					printf("%s %.8x\n", c_inst.sname, ra);
				if (execute) {
					vm->stack[vm->regs.ps++] = (u32) ra;
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
		case INVALID_OPCODE:
			{
				//XXX pass opcodes and continue
				//XXX vm->regs.pc++;
				if (verbose)
					printf("%s %.8x\n", c_inst.sname,
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
	assert(b != 0);
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
