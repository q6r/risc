/* - qnix
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/syscall.h>
#include "vm.h"
#include "compiler.h"

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
