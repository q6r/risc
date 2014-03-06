/* - qnix
 * TODO: implement immu pushi*, popi*
 * =====================================================================================
 * Implementation of a simple virtual machine 32bit
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "emu.h"

bool init_vm(vm_t *vm, size_t cs, size_t ss) {
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
	vm->code = (u8 *)malloc(sizeof(u8)*vm->code_size);
	assert(vm->code != NULL);
	vm->stack = (u8 *)malloc(sizeof(int8_t)*vm->stack_size);
	assert(vm->stack != NULL);
	// zero code and stack
	memset(vm->code, 0x00, cs);
	memset(vm->stack, 0x00, ss);

	return true;
}

bool term_vm(vm_t *vm) {
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

enum INST_NAME get_inst_name(u8 opcode) {
	size_t inst_table_size = sizeof(inst_table) / sizeof(inst_table[0]);
	size_t i;
	for(i=0;i<inst_table_size;i++) {
		if(inst_table[i].opcode == opcode)
			return inst_table[i].name;
	}
	return INVALID_OPCODE;
}

reg_t *ridx_to_rvm(reg_t r_idx, vm_t *vm) {
	reg_t *res = NULL;

	switch(r_idx) {
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

bool process_code(vm_t *vm) {
	for(vm->regs.pc = 0; vm->regs.pc < (reg_t)vm->code_size; ) {
		u8 opcode = vm->code[vm->regs.pc];
		inst c_inst = get_instruction(opcode);
		SHOW_REGS;
		switch(c_inst.name) {
			case ADD:
			{
				printf("add\n");
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01 || rb_idx == 0x02 || rb_idx == 0x03);
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
			    printf("addib\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8)rab);
				break;
			}
			case ADDIW:
			{
			    printf("addiw\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u16)rab);
			   break;
			}
			case ADDID:
			{
			    printf("addid\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u32)rab);
			   break;
			}
			case SUB:
			{
				printf("sub\n");
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01 || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
			case SUBIB:
			{
			    printf("subib\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8)rab);
				break;
			}
			case SUBIW:
			{
			    printf("subiw\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u16 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u16)rab);
			   break;
			}
			case SUBID:
			{
			    printf("subid\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u32 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u32)rab);
			   break;
			}
			case MUL:
			{
				printf("mul\n");
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01 || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
			case MULIB:
			{
			    printf("mulib\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8)rab);
				break;
			}
			case MULIW:
			{
			   printf("muliw\n");
			   // extract register and byte
			   reg_t ra_idx = vm->code[++vm->regs.pc];
			   u16 rab = vm->code[++vm->regs.pc];
			   assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
			   // does job	
			   reg_t *ra = ridx_to_rvm(ra_idx, vm);
			   assert(ra != NULL);
		 	   *ra = c_inst.func(*ra, (u16)rab);
			   break;
			}
			case MULID:
			{
			   printf("mulid\n");
			   // extract register and byte
			   reg_t ra_idx = vm->code[++vm->regs.pc];
			   u32 rab = vm->code[++vm->regs.pc];
			   assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
			   // does job	
			   reg_t *ra = ridx_to_rvm(ra_idx, vm);
			   assert(ra != NULL);
		 	   *ra = c_inst.func(*ra, (u32)rab);
			   break;
			}
			case DIV:
			{
				printf("div\n");
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01 || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
			case DIVIB:
			{
			    printf("divib\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8)rab);
				break;
			}
			case DIVIW:
			{
			   printf("diviw\n");
			   // extract register and byte
			   reg_t ra_idx = vm->code[++vm->regs.pc];
			   u16 rab = vm->code[++vm->regs.pc];
			   assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
			   // does job	
			   reg_t *ra = ridx_to_rvm(ra_idx, vm);
			   assert(ra != NULL);
			   *ra = c_inst.func(*ra, (u16)rab);
			   break;
			}
			case DIVID:
			{
			   printf("divid\n");
			   // extract register and byte
			   reg_t ra_idx = vm->code[++vm->regs.pc];
			   u32 rab = vm->code[++vm->regs.pc];
			   assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
			   // does job	
			   reg_t *ra = ridx_to_rvm(ra_idx, vm);
			   assert(ra != NULL);
			   *ra = c_inst.func(*ra, (u32)rab);
			   break;
			}
			case XOR:
			{
				printf("xor\n");
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01 || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
			case XORIB:
			{
			    printf("xorib\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8)rab);
				break;
			}
			case XORIW:
			{
			   printf("xoriw\n");
			   // extract register and byte
			   reg_t ra_idx = vm->code[++vm->regs.pc];
			   u16 rab = vm->code[++vm->regs.pc];
			   assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
			   // does job	
			   reg_t *ra = ridx_to_rvm(ra_idx, vm);
			   assert(ra != NULL);
			   *ra = c_inst.func(*ra, (u16)rab);
			   break;
			}
			case XORID:
			{
			   printf("xorid\n");
			   // extract register and byte
			   reg_t ra_idx = vm->code[++vm->regs.pc];
			   u32 rab = vm->code[++vm->regs.pc];
			   assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
			   // does job	
			   reg_t *ra = ridx_to_rvm(ra_idx, vm);
			   assert(ra != NULL);
			   *ra = c_inst.func(*ra, (u32)rab);
			   break;
			}
			case MOV:
			{
				printf("mov\n");
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				reg_t rb_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				assert(rb_idx == 0x00 || rb_idx == 0x01 || rb_idx == 0x02 || rb_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				reg_t *rb = ridx_to_rvm(rb_idx, vm);
				assert(ra != NULL);
				assert(rb != NULL);
				*ra = c_inst.func(*ra, *rb);
				break;
			}
			case MOVIB:
			{
			    printf("movib\n");
				// extract register and byte
				reg_t ra_idx = vm->code[++vm->regs.pc];
				u8 rab = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				// does job	
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, (u8)rab);
				break;
			}
			case MOVIW:
			{
			   printf("moviw\n");
			   // extract register and byte
			   reg_t ra_idx = vm->code[++vm->regs.pc];
			   u16 rab = vm->code[++vm->regs.pc];
			   assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
			   // does job	
			   reg_t *ra = ridx_to_rvm(ra_idx, vm);
			   assert(ra != NULL);
			   *ra = c_inst.func(*ra, (u16)rab);
			   break;
			}
			case MOVID:
			{
			   printf("movid\n");
			   // extract register and byte
			   reg_t ra_idx = vm->code[++vm->regs.pc];
			   u32 rab = vm->code[++vm->regs.pc];
			   assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
			   // does job	
			   reg_t *ra = ridx_to_rvm(ra_idx, vm);
			   assert(ra != NULL);
			   *ra = c_inst.func(*ra, (u32)rab);
			   break;
			}
			case INC:
			{
				printf("inc\n");
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
		        reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, 0);
				break;
		    }
			case DEC:
			{
				printf("dec\n");
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
		        reg_t *ra = ridx_to_rvm(ra_idx, vm);
				assert(ra != NULL);
				*ra = c_inst.func(*ra, 0);
				break;
		    }
			case PUSHB:
			{
				printf("PUSHB\n");
				// extract registers
				reg_t ra_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
		        reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->stack[vm->regs.ps++] = (u8)*ra;
				break;
			}
			case PUSHW:
			{
				printf("PUSHW\n");
				reg_t ra_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
		        reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->stack[vm->regs.ps++] = (u16)*ra;
				break;
			}
			case PUSHD:
			{
				printf("PUSHD\n");
				reg_t ra_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
		        reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->stack[vm->regs.ps++] = (u32)*ra;
				break;
			}
			case POPB:
			{
				printf("POPB\n");
				assert(vm->regs.ps > 0);
				reg_t ra_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->regs.ps--;
				*ra = (u8)vm->stack[vm->regs.ps];
				vm->stack[vm->regs.ps] = 0x00000000;
				break;
			}
			case POPW:
			{
				printf("POPW\n");
				assert(vm->regs.ps >= 0);
				reg_t ra_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->regs.ps--;
				*ra = (u16)vm->stack[vm->regs.ps];
				vm->stack[vm->regs.ps] = 0x00000000;
				break;
			}
			case POPD:
			{
				printf("POPD\n");
				assert(vm->regs.ps > 0);
				reg_t ra_idx = vm->code[++vm->regs.pc];
				assert(ra_idx == 0x00 || ra_idx == 0x01 || ra_idx == 0x02 || ra_idx == 0x03);
				reg_t *ra = ridx_to_rvm(ra_idx, vm);
				vm->regs.ps--;
				*ra = (u32)vm->stack[vm->regs.ps];
				break;
			}
			case EXIT:
			{
				printf("exit\n");
				return true;
			}
			case INVALID_OPCODE:
				printf("invalid\n");
				//pass opcodes and continue
				//vm->regs.pc++;
				return false;
			default:
				printf("default\n");
				return false;
		};
		vm->regs.pc++;
	}
	return true;
}

inst get_instruction(u8 opcode) {
	size_t inst_table_size = sizeof(inst_table) / sizeof(inst_table[0]);
	size_t i;
	for(i=0;i<inst_table_size;i++) {
		if(inst_table[i].opcode == opcode)
			return inst_table[i];
	}
	return (inst){INVALID_OPCODE, 0xff, NULL}; // TODO INVALID INSTRUCTION
}

reg_t inst_add(reg_t a, reg_t b) { a += b; return a; }
reg_t inst_sub(reg_t a, reg_t b) { a -= b; return a; }
reg_t inst_mul(reg_t a, reg_t b) { a *= b; return a; }
reg_t inst_div(reg_t a, reg_t b) { assert(b != 0); a /= b; return a; }
reg_t inst_xor(reg_t a, reg_t b) { a ^= b; return a; }
reg_t inst_mov(reg_t a, reg_t b) { a = b; return a;  }
reg_t inst_inc(reg_t a, reg_t unused) { a++; return a; }
reg_t inst_dec(reg_t a, reg_t unused) { a--; return a; }
