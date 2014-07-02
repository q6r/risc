#include "process.h"
/*
 * TODO: when pushing registers should push all content rather than
 * doing push{b,w,d} ??? 
 */


bool is_valid_reg(const u8 r_idx) {

    if(r_idx == RR1 || r_idx == RR2 || r_idx == RR3 ||
      r_idx == RR4 || r_idx == RR5 || r_idx == RR6 ||
        r_idx == RR7 || r_idx == RR8 || r_idx == RPS ||
        r_idx == RPC) {
            return true;
        }

    return false;
}

bool process_reg_reg(const inst c_inst, vm_t *vm, const bool verbose) {
    // extract registers
    u8 ra_idx = vm->code[++vm->regs.pc];
    u8 rb_idx = vm->code[++vm->regs.pc];
    reg_t *ra, *rb;
    if (verbose)
        printf("%s >>>>> %s %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx),
        reg_to_str(rb_idx));
    if(!is_valid_reg(ra_idx) || !is_valid_reg(rb_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    rb = ridx_to_rvm(rb_idx, vm);
    if(ra == NULL || rb == NULL)
        return false;
    *ra = c_inst.func(*ra, *rb);
    return true;
}

bool process_reg_immb(const inst c_inst, vm_t *vm, const bool verbose) {
    u8 ra_idx = vm->code[++vm->regs.pc];
    u8 rab;
    reg_t *ra;

    EXTRACT_8_ARR(rab, vm->code, vm->regs.pc);
    if (verbose)
        printf("%s >>>>> %s %s %.8x\n",
          __PRETTY_FUNCTION__, c_inst.sname, reg_to_str(ra_idx),
        rab);
    if(!is_valid_reg(ra_idx))
        return false;
    // does job     
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;
    *ra = c_inst.func(*ra, (u8) rab);
    return true;
}

bool process_reg_immw(const inst c_inst, vm_t *vm, const bool verbose) {
    u8 ra_idx = vm->code[++vm->regs.pc];
    u16 rab;
    reg_t *ra;
    EXTRACT_16_ARR(rab, vm->code, vm->regs.pc);
    if (verbose)
        printf("%s >>>>> %s %s %.8x\n",
          __PRETTY_FUNCTION__, c_inst.sname, reg_to_str(ra_idx),
        rab);
    if(!is_valid_reg(ra_idx))
        return false;
    // does job
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;
    *ra = c_inst.func(*ra, (u16) rab);
    return true;

}
bool process_reg_immd(const inst c_inst, vm_t *vm, const bool verbose) {
    u8 ra_idx = vm->code[++vm->regs.pc];
    u32 rab;
    reg_t *ra;

    EXTRACT_32_ARR(rab, vm->code, vm->regs.pc);
    if (verbose)
        printf("%s >>>>> %s %s %.8x\n",
          __PRETTY_FUNCTION__, c_inst.sname, reg_to_str(ra_idx),
        rab);
    if(!is_valid_reg(ra_idx))
        return false;
    // does job     
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;
    *ra = c_inst.func(*ra, (u32) rab);
    return true;
}

bool process_reg(const inst c_inst, vm_t *vm, const bool verbose) { 
    // extract registers
    u8 ra_idx = vm->code[++vm->regs.pc];
    reg_t *ra;
    if (verbose)
        printf("%s >>>>> %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx));
    if(!is_valid_reg(ra_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;
    *ra = c_inst.func(*ra, 0);
    return true;
}

bool process_pushb(inst c_inst, vm_t *vm, bool verbose) { 
    // extract registers
    u8 ra_idx = vm->code[++vm->regs.pc];
    reg_t *ra;
    if (verbose)
        printf("%s >>>>> %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx));
    if(!is_valid_reg(ra_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;
    vm->stack[vm->regs.ps++] = (u8) * ra;
    return true;
}

bool process_pushw(inst c_inst, vm_t *vm, bool verbose) { 
    u8 ra_idx = vm->code[++vm->regs.pc];
    reg_t *ra;
    if (verbose)
        printf("%s >>>>> %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx));
    if(!is_valid_reg(ra_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;

    vm->stack[vm->regs.ps++] = (u16) * ra;
    return true;
}
bool process_pushd(inst c_inst, vm_t *vm, bool verbose) {
    u8 ra_idx = vm->code[++vm->regs.pc];
    reg_t *ra;
    if (verbose)
        printf("%s >>>>> %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx));
    if(!is_valid_reg(ra_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;
    vm->stack[vm->regs.ps++] = (u32) * ra;
    return true;
}

bool process_popb(inst c_inst, vm_t *vm, bool verbose) { 
    if(vm->regs.ps <= 0)
        return false;

    u8 ra_idx = vm->code[++vm->regs.pc];
    reg_t *ra;

    if (verbose)
        printf("%s >>>>> %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx));
    if(!is_valid_reg(ra_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;

    vm->regs.ps--;
    *ra = (u8) vm->stack[vm->regs.ps];
    vm->stack[vm->regs.ps] = 0x00000000;
    return true;
}
bool process_popw(inst c_inst, vm_t *vm, bool verbose) { 
    if(vm->regs.ps <= 0)
        return false;

    u8 ra_idx = vm->code[++vm->regs.pc];
    reg_t *ra;

    if (verbose)
        printf("%s >>>>> %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx));
    if(!is_valid_reg(ra_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;

    vm->regs.ps--;
    *ra = (u16) vm->stack[vm->regs.ps];
    vm->stack[vm->regs.ps] = 0x00000000;
    return true;
}
bool process_popd(inst c_inst, vm_t *vm, bool verbose) { 
    reg_t *ra;

    if(vm->regs.ps <= 0)
        return false;
    u8 ra_idx = vm->code[++vm->regs.pc];
    if (verbose)
        printf("%s >>>>> %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx));
    if(!is_valid_reg(ra_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;

    vm->regs.ps--;
    *ra = (u32) vm->stack[vm->regs.ps];
    return true;
}


bool process_pushib(inst c_inst, vm_t *vm, bool verbose) { 
    reg_t ra = vm->code[++vm->regs.pc];
    if (verbose)
        printf("%s >>>>> %s %.8x\n", __PRETTY_FUNCTION__, c_inst.sname,
          ra);
    vm->stack[vm->regs.ps++] = (u8) ra;
    return true;
}

bool process_pushiw(inst c_inst, vm_t *vm, bool verbose) { 
    u16 ra;
    EXTRACT_16_ARR(ra, vm->code, vm->regs.pc);
    if (verbose)
        printf("%s >>>>> %s %.8x\n", __PRETTY_FUNCTION__, c_inst.sname,
          ra);
    vm->stack[vm->regs.ps++] = (u16) ra;
    return true;

}

bool process_pushid(inst c_inst, vm_t *vm, bool verbose) { 
    u32 ra;
    EXTRACT_32_ARR(ra, vm->code, vm->regs.pc);
    if (verbose)
        printf("%s >>>>> %s %.8x\n", __PRETTY_FUNCTION__, c_inst.sname,
          ra);
    vm->stack[vm->regs.ps++] = (u32) ra;
    return true;
}

bool process_gstk(inst c_inst, vm_t *vm, bool verbose) { 
    reg_t *ra;
    if(vm->regs.ps <= 0)
        return false;
    u8 ra_idx = vm->code[++vm->regs.pc];
    if (verbose)
        printf("%s >>>>> %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx));
    if(!is_valid_reg(ra_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;
    *ra = &vm->stack[vm->regs.ps - 1];
    return true;
}

bool process_pstk(inst c_inst, vm_t *vm, bool verbose) { 
    reg_t *ra;
    u8 ra_idx = vm->code[++vm->regs.pc];
    if (verbose)
        printf("%s >>>>> %s %s\n", __PRETTY_FUNCTION__, c_inst.sname,
          reg_to_str(ra_idx));
    if(!is_valid_reg(ra_idx))
        return false;
    ra = ridx_to_rvm(ra_idx, vm);
    if(ra == NULL)
        return false;
    vm->stack[vm->regs.ps++] = *(u32 *) (*ra);
    return true;
}

bool process_exit(inst c_inst, vm_t *vm, bool verbose) {
    if (verbose)
        printf("%s >>> %s\n", __PRETTY_FUNCTION__, c_inst.sname);
    return false;
}


bool process_syscall(reg_t scall, reg_t nargs, inst c_inst, vm_t *vm, bool verbose) {
    if (nargs == -1) {
        printf("%s >>> Invalid system call\n", __PRETTY_FUNCTION__);
        return false;
    }
    printf("%s >>> %s_%.2x(%d arguments)\n",
      __PRETTY_FUNCTION__, c_inst.sname, scall, nargs);
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

    return true;
}


bool process_invalid(inst c_inst, vm_t *vm, bool verbose) {
    // XXX incase of an invalid opcode shall we
    // -ignore and continue ?
    if (verbose)
        printf("%s >>>>> %s %.8x\n", __PRETTY_FUNCTION__, c_inst.sname,
          c_inst.opcode);
    return false;
}
