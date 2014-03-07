/* - qnix
 * =====================================================================
 * Implementation of a simple virtual machine 32bit
 * =====================================================================
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
	PUSHIB, PUSHIW, PUSHID,	/* immu(32,16,8) */
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
bool process_code(vm_t * vm, bool execute, bool verbose);

enum INST_NAME get_inst_name(u8 opcode);
inst get_instruction(u8 opcode);
reg_t *ridx_to_rvm(reg_t r_idx, vm_t * vm);

const char *reg_to_str(reg_t r);

#endif
