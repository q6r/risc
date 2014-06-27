#ifndef PROCESS_H_
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "vm.h"

#define EXTRACT_8_ARR(X, Y, Z) X = (X << 8) | Y[++Z]
#define EXTRACT_16_ARR(X, Y, Z) do { \
			EXTRACT_8_ARR(X, Y, Z); \
			EXTRACT_8_ARR(X, Y, Z); \
			} while(0)
#define EXTRACT_32_ARR(X, Y, Z) do { \
			EXTRACT_16_ARR(X, Y, Z); \
			EXTRACT_16_ARR(X, Y, Z); \
			} while(0)

#define IS_VALID_REG(x) assert(x == RR1 || x == RR2 || \
  x == RR3 || x == RR4 || x == RR5 || x == RR6 || \
  x == RR7 || x == RR8 || x == RPS || x == RPC)

bool is_valid_reg(u8 r_idx);

bool process_add(inst c_inst, vm_t *vm, bool verbose);
bool process_addib(inst c_inst, vm_t *vm, bool verbose);
bool process_addiw(inst c_inst, vm_t *vm, bool verbose);
bool process_addid(inst c_inst, vm_t *vm, bool verbose);

bool process_sub(inst c_inst, vm_t *vm, bool verbose);
bool process_subib(inst c_inst, vm_t *vm, bool verbose);
bool process_subiw(inst c_inst, vm_t *vm, bool verbose);
bool process_subid(inst c_inst, vm_t *vm, bool verbose);


bool process_mul(inst c_inst, vm_t *vm, bool verbose);
bool process_mulib(inst c_inst, vm_t *vm, bool verbose);
bool process_muliw(inst c_inst, vm_t *vm, bool verbose);
bool process_mulid(inst c_inst, vm_t *vm, bool verbose);

bool process_div(inst c_inst, vm_t *vm, bool verbose);
bool process_divib(inst c_inst, vm_t *vm, bool verbose);
bool process_diviw(inst c_inst, vm_t *vm, bool verbose);
bool process_divid(inst c_inst, vm_t *vm, bool verbose);

bool process_xor(inst c_inst, vm_t *vm, bool verbose);
bool process_xorib(inst c_inst, vm_t *vm, bool verbose);
bool process_xoriw(inst c_inst, vm_t *vm, bool verbose);
bool process_xorid(inst c_inst, vm_t *vm, bool verbose);

bool process_mov(inst c_inst, vm_t *vm, bool verbose);
bool process_movib(inst c_inst, vm_t *vm, bool verbose);
bool process_moviw(inst c_inst, vm_t *vm, bool verbose);
bool process_movid(inst c_inst, vm_t *vm, bool verbose);

bool process_inc(inst c_inst, vm_t *vm, bool verbose);
bool process_dec(inst c_inst, vm_t *vm, bool verbose);


bool process_pushb(inst c_inst, vm_t *vm, bool verbose);
bool process_pushw(inst c_inst, vm_t *vm, bool verbose);
bool process_pushd(inst c_inst, vm_t *vm, bool verbose);

bool process_popb(inst c_inst, vm_t *vm, bool verbose);
bool process_popw(inst c_inst, vm_t *vm, bool verbose);
bool process_popd(inst c_inst, vm_t *vm, bool verbose);

bool process_pushib(inst c_inst, vm_t *vm, bool verbose);
bool process_pushiw(inst c_inst, vm_t *vm, bool verbose);
bool process_pushid(inst c_inst, vm_t *vm, bool verbose);


bool process_gstk(inst c_inst, vm_t *vm, bool verbose);
bool process_pstk(inst c_inst, vm_t *vm, bool verbose);

bool process_exit(inst c_inst, vm_t *vm, bool verbose);

bool process_syscall(reg_t scall, reg_t nargs, inst c_inst, vm_t *vm, bool verbose);

bool process_invalid(inst c_inst, vm_t *vm, bool verbose);
#endif
