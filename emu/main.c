/* - qnix
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "emu.h"

const uint8_t code[] = {
// SET TWO TEST 8bit
	0xb1, 0x00, 0xff,	/* addib r1, 0xff */
	0xb1, 0x01, 0xff,	/* addib r2, 0xff */
	0xb1, 0x02, 0xff,	/* addib r3, 0xff */
	0xb1, 0x03, 0xff,	/* addib r4, 0xff */
	0xb2, 0x00, 0xff,	/* subib r1, 0xff */
	0xb2, 0x01, 0xff,	/* subib r2, 0xff */
	0xb2, 0x02, 0xff,	/* subib r3, 0xff */
	0xb2, 0x03, 0xff,	/* subib r4, 0xff */
	0xb3, 0x00, 0xff,	/* mulib r1, 0xff */
	0xb3, 0x01, 0xff,	/* mulib r2, 0xff */
	0xb3, 0x02, 0xff,	/* mulib r3, 0xff */
	0xb3, 0x03, 0xff,	/* mulib r4, 0xff */
	0xb4, 0x00, 0xff,	/* divib r1, 0xff */
	0xb4, 0x01, 0xff,	/* divib r2, 0xff */
	0xb4, 0x02, 0xff,	/* divib r3, 0xff */
	0xb4, 0x03, 0xff,	/* divib r4, 0xff */
	0xb5, 0x00, 0xff,	/* xorib r1, 0xff */
	0xb5, 0x01, 0xff,	/* xorib r2, 0xff */
	0xb5, 0x02, 0xff,	/* xorib r3, 0xff */
	0xb5, 0x03, 0xff,	/* xorib r4, 0xff */
	0xb6, 0x00, 0xff,	/* movib r1, 0xff */
	0xb6, 0x01, 0xff,	/* movib r2, 0xff */
	0xb6, 0x02, 0xff,	/* movib r3, 0xff */
	0xb6, 0x03, 0xff,	/* movib r4, 0xff */
	0xaf			/* exit */
// SET ONE TEST
//  0xa7, 0x00, /* inc r1 */
//  0xa7, 0x00, /* inc r1 */
//  0xa7, 0x00, /* inc r1 */
//  0xa7, 0x00, /* inc r1 */
//  0xa7, 0x00, /* inc r1 */
//  0xa7, 0x00, /* inc r1 */
//  0xa7, 0x01, /* inc r2 */
//  0xa7, 0x01, /* inc r2 */
//  0xa7, 0x01, /* inc r2 */
//  0xa7, 0x01, /* inc r2 */
//  0xa7, 0x01, /* inc r2 */
//  0xa7, 0x01, /* inc r2 */
//  0xa7, 0x01, /* inc r2 */
//  0xa7, 0x01, /* inc r2 */
//  0xa7, 0x02, /* inc r3 */
//  0xa7, 0x02, /* inc r3 */
//  0xa7, 0x02, /* inc r3 */
//  0xa7, 0x02, /* inc r3 */
//  0xa7, 0x02, /* inc r3 */
//  0xa7, 0x02, /* inc r3 */
//  0xa7, 0x02, /* inc r3 */
//  0xa7, 0x02, /* inc r3 */
//  0xa7, 0x02, /* inc r3 */
//  0xa7, 0x03, /* inc r4 */
//  0xa7, 0x03, /* inc r4 */
//  0xa7, 0x03, /* inc r4 */
//  0xa7, 0x03, /* inc r4 */
//  0xa7, 0x03, /* inc r4 */
//  0xa7, 0x03, /* inc r4 */
//  0xa7, 0x03, /* inc r4 */
//  0xa7, 0x03, /* inc r4 */
//  0xa7, 0x03, /* inc r4 */
//  0xa9, 0x00, /* pushb r1 */
//  0xaa, 0x00, /* pushw r1 */
//  0xab, 0x00, /* pushd r1 */
//  0xac, 0x00, /* popb r1 */
//  0xad, 0x00, /* popw r1 */
//  0xae, 0x00, /* popd r1 */
//  0xa1, 0x00, 0x01, /* add r1, r2 */
//  0xa1, 0x02, 0x03, /* add r2, r3 */
//  0xa2, 0x00, 0x01, /* sub r1, r2 */
//  0xa2, 0x02, 0x03, /* sub r3, r4 */
//  0xa3, 0x00, 0x01, /* mul r1, r2 */
//  0xa3, 0x02, 0x03, /* mul r3, r4 */
//  0xa4, 0x00, 0x01, /* div r1, r2 */
//  0xa4, 0x02, 0x03, /* div r3, r4 */
//  0xa5, 0x00, 0x01, /* xor r1, r2 */
//  0xa5, 0x02, 0x03, /* xor r3, r4 */
//  0xa6, 0x00, 0x01, /* mov r1, r2 */
//  0xa6, 0x02, 0x03, /* mov r3, r4 */
//  0xaf              /* exit */
};

int main()
{
	vm_t vm;
	init_vm(&vm, 1024, 1024);
	memcpy(vm.code, code, sizeof code);
	if (process_code(&vm) == false) {
		printf("Failed processing code\n");
	}
	term_vm(&vm);
	return 0;
}
