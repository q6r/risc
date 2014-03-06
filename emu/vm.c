/* - qnix
 * TODO: implement immu pushi*, popi*
 * TODO: registers numbers should be considered as u8 not reg_t
 * TODO: maybe convert reg_t to a struct that contains a value, and a string name
 * and maybe other things.. settings ? -reg_to_str(..)
 * TODO: add more cases to test the rest of the instructions
 * maybe use _Check_
 * =====================================================================================
 * Implementation of a simple virtual machine 32bit
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "emu.h"

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

bool process_code(vm_t * vm)
{
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
				printf("%s %s %s\n", c_inst.sname,
				       reg_to_str(ra_idx), reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
				// register index to vm register
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				// do instruction
				*ra = c_inst.func(*ra, *rb);
				break;
			}
		case ADDIB:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8) rab);
				break;
			}
		case ADDIW:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u16) rab);
				break;
			}
		case ADDID:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u32) rab);
				break;
			}
		case SUB:
			{
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				printf("%s %s %s\n", c_inst.sname,
				       reg_to_str(ra_idx), reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
		case SUBIB:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8) rab);
				break;
			}
		case SUBIW:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u16) rab);
				break;
			}
		case SUBID:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u32) rab);
				break;
			}
		case MUL:
			{
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				printf("%s %s %s\n", c_inst.sname,
				       reg_to_str(ra_idx), reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
		case MULIB:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8) rab);
				break;
			}
		case MULIW:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job  
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u16) rab);
				break;
			}
		case MULID:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job  
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u32) rab);
				break;
			}
		case DIV:
			{
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				printf("%s %s %s\n", c_inst.sname,
				       reg_to_str(ra_idx), reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
		case DIVIB:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8) rab);
				break;
			}
		case DIVIW:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job  
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u16) rab);
				break;
			}
		case DIVID:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job  
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u32) rab);
				break;
			}
		case XOR:
			{
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				printf("%s %s %s\n", c_inst.sname,
				       reg_to_str(ra_idx), reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
		case XORIB:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8) rab);
				break;
			}
		case XORIW:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job  
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u16) rab);
				break;
			}
		case XORID:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job  
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u32) rab);
				break;
			}
		case MOV:
			{
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				printf("%s %s %s\n", c_inst.sname,
				       reg_to_str(ra_idx), reg_to_str(rb_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01
				       || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
		case MOVIB:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job     
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8) rab);
				break;
			}
		case MOVIW:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job  
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u16) rab);
				break;
			}
		case MOVID:
			{
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab = vm->code[++vm->regs.pc];
				printf("%s %s %.8x\n", c_inst.sname,
				       reg_to_str(ra_idx), rab);
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				// does job  
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u32) rab);
				break;
			}
		case INC:
			{
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				printf("%s %s\n", c_inst.sname,
				       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, 0);
				break;
			}
		case DEC:
			{
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				printf("%s %s\n", c_inst.sname,
				       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, 0);
				break;
			}
		case PUSHB:
			{
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				printf("%s %s\n", c_inst.sname,
				       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->stack[vm->regs.ps++] = (u8) * ra;
				break;
			}
		case PUSHW:
			{
				reg_t ra_idx = vm->code[++vm->regs.pc];
				printf("%s %s\n", c_inst.sname,
				       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->stack[vm->regs.ps++] = (u16) * ra;
				break;
			}
		case PUSHD:
			{
				reg_t ra_idx = vm->code[++vm->regs.pc];
				printf("%s %s\n", c_inst.sname,
				       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->stack[vm->regs.ps++] = (u32) * ra;
				break;
			}
		case POPB:
			{
				assert(vm->regs.ps > 0);
				reg_t ra_idx = vm->code[++vm->regs.pc];
				printf("%s %s\n", c_inst.sname,
				       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->regs.ps--;
				*ra = (u8) vm->stack[vm->regs.ps];
				vm->stack[vm->regs.ps] = 0x00000000;
				break;
			}
		case POPW:
			{
				assert(vm->regs.ps >= 0);
				reg_t ra_idx = vm->code[++vm->regs.pc];
				printf("%s %s\n", c_inst.sname,
				       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->regs.ps--;
				*ra = (u16) vm->stack[vm->regs.ps];
				vm->stack[vm->regs.ps] = 0x00000000;
				break;
			}
		case POPD:
			{
				assert(vm->regs.ps > 0);
				reg_t ra_idx = vm->code[++vm->regs.pc];
				printf("%s %s\n", c_inst.sname,
				       reg_to_str(ra_idx));
				assert(ra_idx == 0x00 || ra_idx == 0x01
				       || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->regs.ps--;
				*ra = (u32) vm->stack[vm->regs.ps];
				break;
			}
		case EXIT:
			{
				printf("%s\n", c_inst.sname);
				return true;
			}
		case INVALID_OPCODE:
			//XXX pass opcodes and continue
			//XXX vm->regs.pc++;
			printf("%s\n", c_inst.sname);
			return false;
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
	INVALID_OPCODE, "InvalidOp", 0xff, NULL};	// TODO INVALID INSTRUCTION
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
