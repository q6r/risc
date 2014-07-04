#ifndef compiler_H
#define compiler_H

#include "vm.h"

u8 *read_file_content(char *file_name);
u8 *assemble(u8 *file_content);
u8 *compile(char *file_name);

#endif // compiler_H
