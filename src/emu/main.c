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

#define MACHINE_SZ 1024*8

int main(int argc, char **argv)
{
    vm_t vm;
    u8 *stdin_buf;
    size_t read_size;

    stdin_buf = calloc(MACHINE_SZ, sizeof(u8));    
    if(stdin_buf == NULL) {
      perror("calloc");
      return -1;
    }

    read_size = fread(stdin_buf, sizeof(u8), MACHINE_SZ, stdin);
    if(read_size <= 0) {
        fprintf(stderr, "[-] fread: failed\n");
        return -1;
    }

    if (!init_vm(&vm, read_size, read_size)) {
        fprintf(stderr,"[-] init_vm: failed\n");
        return -1;
    }

    memcpy(vm.code, stdin_buf, read_size);
    if (!process_code(&vm, true)) {
        fprintf(stderr, "[-] process_code: failed\n");
        term_vm(&vm);
        return -1;
    } else {
        term_vm(&vm);
    }

    return 0;
}
