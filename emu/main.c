/* - qnix
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <config.h>
#include <sys/syscall.h>
#include "vm.h"

const uint8_t code[] = {	// XXX None of thse should fail assert
    // SET 6 TEST FOR CALLS
    OP_MOVIB, RR1, SYS_write,	/* r1 = sys_write */
    OP_MOVIB, RR2, 0x00,	/* r2 = stdout */
    OP_PUSHID, 0x6c, 0x6c, 0x65, 0x68,	/* hell */
    OP_GSTK, RR3,		/* stack ptr to string */
    OP_PUSHID, 0x72, 0x6f, 0x77, 0x20,	/*  worl */
    OP_PUSHID, 0x00, 0x0a, 0x64, 0x6c,	/* d\n\0 */
    OP_MOVIB, RR4, 11,	/* size */
    OP_SYSCALL,		/* syscall */
    OP_MOVIB, RR1, SYS_exit,	// r1 = sys_exit
    OP_MOVIB, RR2, 0xff,	// r2 = 0xff
    OP_SYSCALL,		// r1 = sys_exit(r2);
    // SET 5 TEST FOR STACK
    OP_PUSHID, 0x11, 0x11, 0x11, 0x11,
    OP_PUSHID, 0x22, 0x22, 0x22, 0x22,
    OP_PUSHIW, 0x33, 0x33,
    OP_PUSHIW, 0x44, 0x44,
    OP_PUSHIB, 0x55,
    OP_PUSHIB, 0x66,
    OP_POPB, RR1,
    OP_POPB, RR2,
    OP_POPW, RR3,
    OP_POPW, RR4,
    OP_POPD, RR5,
    OP_POPD, RR6,
    OP_PUSHB, RR6,
    OP_PUSHW, RR6,
    OP_PUSHD, RR6,
    OP_POPD, RR1,
    OP_POPD, RR1,
    OP_POPD, RR1,
    //OP_POPD, RR2, // XXX Nothing to pop better handling
    OP_PUSHID, 0x11, 0x11, 0x11, 0x11,
    OP_PUSHID, 0x22, 0x22, 0x22, 0x22,
    OP_PUSHID, 0x33, 0x33, 0x33, 0x33,
    OP_PUSHID, 0x44, 0x44, 0x44, 0x44,
    OP_GSTK, RR1,		// Should contain pointer in stack to 0x44444444
    OP_DEC, RR1,
    OP_DEC, RR1,
    OP_DEC, RR1,
    OP_DEC, RR1,
    OP_PSTK, RR1,		// Should put 0x333333 into stack
    //      OP_EXIT,
    // SET 4 TEST 32bit
    OP_ADDID, RR1, 0x11, 0x22, 0x33, 0x44,	/* addid r1, 0x11223344 */
    OP_ADDID, RR2, 0x11, 0x22, 0x33, 0x44,	/* addid r2, 0x11223344 */
    OP_ADDID, RR3, 0x11, 0x22, 0x33, 0x44,	/* addid r3, 0x11223344 */
    OP_ADDID, RR4, 0x11, 0x22, 0x33, 0x44,	/* addid r4, 0x11223344 */
    OP_SUBID, RR1, 0x11, 0x22, 0x33, 0x44,	/* subid r1, 0x11223344 */
    OP_SUBID, RR2, 0x11, 0x22, 0x33, 0x44,	/* subid r2, 0x11223344 */
    OP_SUBID, RR3, 0x11, 0x22, 0x33, 0x44,	/* subid r3, 0x11223344 */
    OP_SUBID, RR4, 0x11, 0x22, 0x33, 0x44,	/* subid r4, 0x11223344 */
    OP_MULID, RR1, 0x11, 0x22, 0x33, 0x44,	/* mulid r1, 0x11223344 */
    OP_MULID, RR2, 0x11, 0x22, 0x33, 0x44,	/* mulid r2, 0x11223344 */
    OP_MULID, RR3, 0x11, 0x22, 0x33, 0x44,	/* mulid r3, 0x11223344 */
    OP_MULID, RR4, 0x11, 0x22, 0x33, 0x44,	/* mulid r4, 0x11223344 */
    OP_DIVID, RR1, 0x11, 0x22, 0x33, 0x44,	/* divid r1, 0x11223344 */
    OP_DIVID, RR2, 0x11, 0x22, 0x33, 0x44,	/* divid r2, 0x11223344 */
    OP_DIVID, RR3, 0x11, 0x22, 0x33, 0x44,	/* divid r3, 0x11223344 */
    OP_DIVID, RR4, 0x11, 0x22, 0x33, 0x44,	/* divid r4, 0x11223344 */
    OP_XORID, RR1, 0x11, 0x22, 0x33, 0x44,	/* xorid r1, 0x11223344 */
    OP_XORID, RR2, 0x11, 0x22, 0x33, 0x44,	/* xorid r2, 0x11223344 */
    OP_XORID, RR3, 0x11, 0x22, 0x33, 0x44,	/* xorid r3, 0x11223344 */
    OP_XORID, RR4, 0x11, 0x22, 0x33, 0x44,	/* xorid r4, 0x11223344 */
    OP_MOVID, RR1, 0x11, 0x22, 0x33, 0x44,	/* movid r1, 0x11223344 */
    OP_MOVID, RR2, 0x11, 0x22, 0x33, 0x44,	/* movid r2, 0x11223344 */
    OP_MOVID, RR3, 0x11, 0x22, 0x33, 0x44,	/* movid r3, 0x11223344 */
    OP_MOVID, RR4, 0x11, 0x22, 0x33, 0x44,	/* movid r4, 0x11223344 */
    OP_PUSHID, 0x11, 0x22, 0x33, 0x44,	/* pushid 0x11223344 */
    OP_PUSHD, RR1,		/* pushd r1 */
    OP_PUSHD, RR2,		/* pushd r2 */
    OP_PUSHD, RR3,		/* pushd r3 */
    OP_PUSHD, RR4,		/* pushd r4 */
    OP_PUSHD, RR1,		/* popd r1 */
    OP_PUSHD, RR2,		/* popd r1 */
    OP_PUSHD, RR3,		/* popd r1 */
    OP_PUSHD, RR4,		/* popd r1 */
    // SET 3 TEST 16bit
    OP_ADDIW, RR1, 0x11, 0x22,	/* addiw r1, 0x1122 */
    OP_ADDIW, RR2, 0x11, 0x22,	/* addiw r2, 0x1122 */
    OP_ADDIW, RR3, 0x11, 0x22,	/* addiw r3, 0x1122 */
    OP_ADDIW, RR4, 0x11, 0x22,	/* addiw r4, 0x1122 */
    OP_SUBIW, RR1, 0x11, 0x22,	/* subiw r1, 0x1122 */
    OP_SUBIW, RR2, 0x11, 0x22,	/* subiw r2, 0x1122 */
    OP_SUBIW, RR3, 0x11, 0x22,	/* subiw r3, 0x1122 */
    OP_SUBIW, RR4, 0x11, 0x22,	/* subiw r4, 0x1122 */
    OP_MULIW, RR1, 0x11, 0x22,	/* muliw r1, 0x1122 */
    OP_MULIW, RR2, 0x11, 0x22,	/* muliw r2, 0x1122 */
    OP_MULIW, RR3, 0x11, 0x22,	/* muliw r3, 0x1122 */
    OP_MULIW, RR4, 0x11, 0x22,	/* muliw r4, 0x1122 */
    OP_DIVIW, RR1, 0x11, 0x22,	/* diviw r1, 0x1122 */
    OP_DIVIW, RR2, 0x11, 0x22,	/* diviw r2, 0x1122 */
    OP_DIVIW, RR3, 0x11, 0x22,	/* diviw r3, 0x1122 */
    OP_DIVIW, RR4, 0x11, 0x22,	/* diviw r4, 0x1122 */
    OP_XORIW, RR1, 0x11, 0x22,	/* xoriw r1, 0x1122 */
    OP_XORIW, RR2, 0x11, 0x22,	/* xoriw r2, 0x1122 */
    OP_XORIW, RR3, 0x11, 0x22,	/* xoriw r3, 0x1122 */
    OP_XORIW, RR4, 0x11, 0x22,	/* xoriw r4, 0x1122 */
    OP_MOVIW, RR1, 0x11, 0x22,	/* moviw r1, 0x1122 */
    OP_MOVIW, RR2, 0x11, 0x22,	/* moviw r2, 0x1122 */
    OP_MOVIW, RR3, 0x11, 0x22,	/* moviw r3, 0x1122 */
    OP_MOVIW, RR4, 0x11, 0x22,	/* moviw r4, 0x1122 */
    OP_PUSHIW, 0x11, 0x22,	/* pushiw 0x1122 */
    OP_PUSHW, RR1,		/* pushw r1 */
    OP_PUSHW, RR2,		/* pushw r2 */
    OP_PUSHW, RR3,		/* pushw r3 */
    OP_PUSHW, RR4,		/* pushw r4 */
    OP_POPW, RR1,		/* popw r1 */
    OP_POPW, RR2,		/* popw r2 */
    OP_POPW, RR3,		/* popw r3 */
    OP_POPW, RR4,		/* popw r4 */
    // SET 2 TEST 8bit
    OP_ADDIB, RR1, 0xff,	/* addib r1, 0xff */
    OP_ADDIB, RR2, 0xff,	/* addib r2, 0xff */
    OP_ADDIB, RR3, 0xff,	/* addib r3, 0xff */
    OP_ADDIB, RR4, 0xff,	/* addib r4, 0xff */
    OP_SUBIB, RR1, 0xff,	/* subib r1, 0xff */
    OP_SUBIB, RR2, 0xff,	/* subib r2, 0xff */
    OP_SUBIB, RR3, 0xff,	/* subib r3, 0xff */
    OP_SUBIB, RR4, 0xff,	/* subib r4, 0xff */
    OP_MULIB, RR1, 0xff,	/* mulib r1, 0xff */
    OP_MULIB, RR2, 0xff,	/* mulib r2, 0xff */
    OP_MULIB, RR3, 0xff,	/* mulib r3, 0xff */
    OP_MULIB, RR4, 0xff,	/* mulib r4, 0xff */
    OP_DIVIB, RR1, 0xff,	/* divib r1, 0xff */
    OP_DIVIB, RR2, 0xff,	/* divib r2, 0xff */
    OP_DIVIB, RR3, 0xff,	/* divib r3, 0xff */
    OP_DIVIB, RR4, 0xff,	/* divib r4, 0xff */
    OP_XORIB, RR1, 0xff,	/* xorib r1, 0xff */
    OP_XORIB, RR2, 0xff,	/* xorib r2, 0xff */
    OP_XORIB, RR3, 0xff,	/* xorib r3, 0xff */
    OP_XORIB, RR4, 0xff,	/* xorib r4, 0xff */
    OP_MOVIB, RR1, 0xff,	/* movib r1, 0xff */
    OP_MOVIB, RR2, 0xff,	/* movib r2, 0xff */
    OP_MOVIB, RR3, 0xff,	/* movib r3, 0xff */
    OP_MOVIB, RR4, 0xff,	/* movib r4, 0xff */
    OP_PUSHIB, 0xff,	/* pushib 0xff */
    OP_PUSHB, RR1,		/* pushb r1 */
    OP_PUSHB, RR2,		/* pushb r2 */
    OP_PUSHB, RR3,		/* pushb r3 */
    OP_PUSHB, RR4,		/* pushb r4 */
    OP_PUSHD, RR1,		/* popb r1 */
    OP_PUSHD, RR2,		/* popb r2 */
    OP_PUSHD, RR3,		/* popb r3 */
    OP_PUSHD, RR4,		/* popb r4 */
    // SET 1 TEST registers 
    OP_DEC, RR1,		/* dec r1 */
    OP_DEC, RR2,		/* dec r2 */
    OP_DEC, RR3,		/* dec r3 */
    OP_DEC, RR4,		/* dec r4 */
    OP_INC, RR1,		/* inc r1 */
    OP_INC, RR2,		/* inc r2 */
    OP_INC, RR3,		/* inc r3 */
    OP_INC, RR4,		/* inc r4 */
    OP_ADD, RR1, RR2,	/* add r1, r2 */
    OP_ADD, RR3, RR4,	/* add r2, r3 */
    OP_SUB, RR1, RR2,	/* sub r1, r2 */
    OP_SUB, RR3, RR4,	/* sub r3, r4 */
    OP_MUL, RR1, RR2,	/* mul r1, r2 */
    OP_MUL, RR3, RR4,	/* mul r3, r4 */
    OP_DIV, RR1, RR2,	/* div r1, r2 */
    OP_DIV, RR3, RR4,	/* div r3, r4 */
    OP_XOR, RR1, RR2,	/* xor r1, r2 */
    OP_XOR, RR3, RR4,	/* xor r3, r4 */
    OP_MOV, RR1, RR2,	/* mov r1, r2 */
    OP_MOV, RR3, RR4,	/* mov r3, r4 */
    OP_EXIT			/* exit */
};

int main(int argc, char **argv)
{
    vm_t vm;

    if(argc < 2) {
        printf("Usage : %s \"<opcodes>\"\n", argv[0]);
        return -1;
    }

    if (!init_vm(&vm, strlen(argv[1]), strlen(argv[1]))) {
        printf("Unable to initalize vm\n");
        return -1;
    }

    memcpy(vm.code, argv[1], strlen(argv[1]));
    if (!process_code(&vm, true)) {
        printf("Failed processing code\n");
        term_vm(&vm);
        return -1;
    } else {
        term_vm(&vm);
    }

    return 0;
}
