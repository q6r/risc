/* - qnix
 * =====================================================================================
 * Implementation of a simple virtual machine 32bit
 *
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
    ADD, SUB, MUL, DIV, XOR, MOV, INC, DEC,
	PUSHB, PUSHW, PUSHD, POPB, POPW, POPD,
    EXIT, INVALID_OPCODE };

typedef struct {
	enum INST_NAME name;
	u8 opcode;
	u32 (*func)(reg_t a, reg_t b);
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

u32 inst_add(reg_t a, reg_t b);
u32 inst_sub(reg_t a, reg_t b);
u32 inst_mul(reg_t a, reg_t b);
u32 inst_div(reg_t a, reg_t b);
u32 inst_xor(reg_t a, reg_t b);
u32 inst_mov(reg_t a, reg_t b);
u32 inst_inc(reg_t a, reg_t unused);
u32 inst_dec(reg_t a, reg_t unused);
//u32 inst_push_byte(reg_t a, reg_t unused);
//u32 inst_push_word(reg_t a, reg_t unused);
//u32 inst_push_dword(reg_t a, reg_t unused);
//u32 inst_pop_byte(reg_t a, reg_t unused);
//u32 inst_pop_word(reg_t a, reg_t unused);
//u32 inst_pop_dword(reg_t a, reg_t unused);
//u32 inst_exit(reg_t status, reg_t unused);
bool init_vm(vm_t *vm, size_t cs, size_t ss);
bool term_vm(vm_t *vm);
bool process_code(vm_t *vm);

enum INST_NAME get_inst_name(u8 opcode);
inst get_instruction(u8 opcode);
reg_t *ridx_to_rvm(reg_t r_idx, vm_t *vm);

static inst inst_table[100] =
	{
		{ADD, 0xa1, inst_add},
		{SUB, 0xa2, inst_sub},
		{MUL, 0xa3, inst_mul},
		{DIV, 0xa4, inst_div},
		{XOR, 0xa5, inst_xor},
		{MOV, 0xa6, inst_mov},
		{INC, 0xa7, inst_inc},
		{DEC, 0xa8, inst_dec},
		{PUSHB, 0xa9, NULL},
		{PUSHW, 0xaa, NULL},
		{PUSHD, 0xab, NULL},
		{POPB,  0xac, NULL},
		{POPW,  0xad, NULL},
		{POPD,  0xae, NULL},
		{EXIT,  0xaf, NULL}
	};


#endif
