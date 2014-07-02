/* - qnix
 * =====================================================================
 * Implementation of a simple virtual machine 32bit
 * =====================================================================
 */

#ifndef __VM_H__
#define __VM_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/syscall.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef u32 reg_t;

typedef enum {
    ADD, SUB, MUL, DIV, XOR, MOV, INC, DEC,	/* reg(reg_t) */
    ADDIB, SUBIB, MULIB, DIVIB, XORIB, MOVIB,	/* immb(8) */
    ADDIW, SUBIW, MULIW, DIVIW, XORIW, MOVIW,	/* immw(16) */
    ADDID, SUBID, MULID, DIVID, XORID, MOVID,	/* immd(32) */
    PUSHB, PUSHW, PUSHD, POPB, POPW, POPD,	/* reg(reg_t) */
    PUSHIB, PUSHIW, PUSHID,	/* immu(32,16,8) */
    GSTK, PSTK,
    EXIT, SYSCALL, INVALID_OPCODE
} INST_NAME;

typedef struct {
    INST_NAME name;
    char sname[32];
    u8 opcode;
    reg_t(*func) (reg_t a, reg_t b);
} inst;

// opcodes of instrucions
#define OP_ADD 0xa1
#define OP_SUB 0xa2
#define OP_MUL 0xa3
#define OP_DIV 0xa4
#define OP_XOR 0xa5
#define OP_MOV 0xa6
#define OP_INC 0xa7
#define OP_DEC 0xa8
#define OP_PUSHB 0xa9
#define OP_PUSHW 0xaa
#define OP_PUSHD 0xab
#define OP_POPB 0xac
#define OP_POPW 0xad
#define OP_POPD 0xae
#define OP_ADDIB 0xb1
#define OP_SUBIB 0xb2
#define OP_MULIB 0xb3
#define OP_DIVIB 0xb4
#define OP_XORIB 0xb5
#define OP_MOVIB 0xb6
#define OP_PUSHIB 0xd9
#define OP_ADDIW 0xd1
#define OP_SUBIW 0xd2
#define OP_MULIW 0xd3
#define OP_DIVIW 0xd4
#define OP_XORIW 0xd5
#define OP_MOVIW 0xd6
#define OP_PUSHIW 0xda
#define OP_ADDID 0xc1
#define OP_SUBID 0xc2
#define OP_MULID 0xc3
#define OP_DIVID 0xc4
#define OP_XORID 0xc5
#define OP_MOVID 0xc6
#define OP_PUSHID 0xdb
#define OP_EXIT 0xaf
#define OP_SYSCALL 0xff
#define OP_GSTK 0xe1
#define OP_PSTK 0xe2

// Registers references
// XXX : Maybe convert to reg_table[] ?
#define RPC 0x00
#define RPS 0x01
#define RR1 0x02
#define RR2 0x03
#define RR3 0x04
#define RR4 0x05
#define RR5 0x06
#define RR6 0x07
#define RR7 0x08
#define RR8 0x09

typedef struct {
    reg_t pc;		/*  RPC */
    reg_t ps;		/*  RPS */
    reg_t r1,		/*  RR1 */
          r2,			/*  RR2 */
          r3,			/*  RR3 */
          r4,			/*  RR4 */
          r5,			/*  RR5 */
          r6,			/*  RR6 */
          r7,			/*  RR7 */
          r8;			/*  RR8 */
} regs_t;

typedef struct {
    regs_t regs;
    size_t code_size;
    size_t stack_size;
    u8 *code;
    u32 *stack;
} vm_t;

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
bool process_code(vm_t * vm, bool verbose);

INST_NAME get_inst_name(const u8 opcode);
inst get_instruction(const u8 opcode);
reg_t *ridx_to_rvm(reg_t r_idx, vm_t * vm);

const char *reg_to_str(reg_t r);
reg_t get_syscall_nargs(reg_t scall);

bool show_regs(vm_t *vm);

#endif
