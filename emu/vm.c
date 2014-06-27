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
#include <signal.h>
#include "vm.h"
#include "process.h"

#define ARR_SIZE(X) sizeof(X)/sizeof(X[0])

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
    vm->code = calloc(sizeof(u8), vm->code_size);
    if (vm->code == NULL) {
        perror("calloc");
        return false;
    }
    vm->stack = calloc(sizeof(u32), vm->stack_size);
    if (vm->stack == NULL) {
        perror("calloc");
        free(vm->code);
        return false;
    }

    return true;
}

bool term_vm(vm_t * vm)
{
    free(vm->code);
    free(vm->stack);
    return true;
}

INST_NAME get_inst_name(const u8 opcode)
{
    size_t i;
    for (i = 0; i < ARR_SIZE(inst_table); i++) {
        if (inst_table[i].opcode == opcode) {
            return inst_table[i].name;
        }
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
              process_add(c_inst, vm, true);
              break;
           }
        case ADDIB:
           {
              process_addib(c_inst, vm, true);
              break;
           }
        case ADDIW:
           {
              process_addiw(c_inst, vm, true);
              break;
           }
        case ADDID:
           {
              process_addid(c_inst, vm, true);
              break;
           }
        case SUB:
           {
              process_sub(c_inst, vm, true);	
              break;
           }
        case SUBIB:
           {
              process_subib(c_inst, vm, true);
              break;
           }
        case SUBIW:
           {
              process_subiw(c_inst, vm, true);
              break;
           }
        case SUBID:
           {
              process_subid(c_inst, vm, true);
              break;
           }
        case MUL:
           {
              process_mul(c_inst, vm, true);
              break;
           }
        case MULIB:
           {
              process_mulib(c_inst, vm, true);
              break;
           }
        case MULIW:
           {
              process_muliw(c_inst, vm, true);
              break;
           }
        case MULID:
           {
              process_mulid(c_inst, vm, true);
              break;
           }
        case DIV:
           {
              process_div(c_inst, vm, true);
              break;
           }
        case DIVIB:
           {
              process_divib(c_inst, vm, true);
              break;
           }
        case DIVIW:
           {
              process_diviw(c_inst, vm, true);
              break;
           }
        case DIVID:
           {
              process_divid(c_inst, vm, true);
              break;
           }
        case XOR:
           {
              process_xor(c_inst, vm, true);
              break;
           }
        case XORIB:
           {
              process_xorib(c_inst, vm, true);
              break;
           }
        case XORIW:
           {
              process_xoriw(c_inst, vm, true);
              break;
           }
        case XORID:
           {
              process_xorid(c_inst, vm, true);
              break;
           }
        case MOV:
           {
              process_mov(c_inst, vm, true);
              break;
           }
        case MOVIB:
           {
              process_movib(c_inst, vm, true);
              break;
           }
        case MOVIW:
           {
              process_moviw(c_inst, vm, true);
              break;
           }
        case MOVID:
           {
              process_movid(c_inst, vm, true);
              break;
           }
        case INC:
           {
              process_inc(c_inst, vm, true);
              break;
           }
        case DEC:
           {
              process_dec(c_inst, vm, true);
              break;
           }
        case PUSHB:
           {
              process_pushb(c_inst, vm, true);
              break;
           }
        case PUSHW:
           {
              process_pushw(c_inst, vm, true);
              break;
           }
        case PUSHD:
           {
              process_pushd(c_inst, vm, true);
              break;
           }
        case POPB:
           {
              process_popb(c_inst, vm, true);
              break;
           }
        case POPW:
           {
              process_popw(c_inst, vm, true);
              break;
           }
        case POPD:
           {
              process_popd(c_inst, vm, true);
              break;
           }
        case PUSHIB:
           {
              process_pushib(c_inst, vm, true);
              break;
           }
        case PUSHIW:
           {
              process_pushiw(c_inst, vm, true);
              break;
           }
        case PUSHID:
           {
              process_pushid(c_inst, vm, true);
              break;
           }
        case GSTK:
           {
              process_gstk(c_inst, vm, true);
              break;
           }
        case PSTK:
           {
              process_gstk(c_inst, vm, true);
              break;
           }
        case EXIT:
           {
              process_exit(c_inst, vm, true);
              break;
           }
        case SYSCALL:
           {
              reg_t scall = vm->regs.r1;
              reg_t nargs = get_syscall_nargs(scall);
              process_syscall(scall, nargs, c_inst, vm, true);
              break;
           }
        case INVALID_OPCODE:
           {
              process_invalid(c_inst, vm, true);
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

inst get_instruction(const u8 opcode)
{
    size_t i;
    for (i = 0; i < ARR_SIZE(inst_table); i++) {
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
    if(b == 0) {
        raise(SIGFPE);
        abort();
    }

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
