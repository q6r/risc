#ifndef compiler_H
#define compiler_H

#include <stdio.h>
#include "vm.h"
#include <assert.h>

typedef enum { REG_REG, REG_IMMB, REG_IMMW, REG_IMMD, REG, IMMB, IMMW, IMMD } OP_TYPE;

typedef struct {
  u8 op;
  char op_name[256];
  OP_TYPE op_type;
  //u8 max;
} asm_table_t;

u8 *read_file_content(char *file_name);
u8 *compile(char *file_name);

bool assemble(u8 *file_content);
char **split_str(char *content, const char *delim);
char *translate_line(char *line);
asm_table_t *get_op_from_table(char *opname);
u8 reg_str_to_byte(const char *str);
u8 immb_str_to_byte(const char *str);
u16 immw_str_to_byte(const char *str);
u32 immd_str_to_byte(const char *str);

#endif // compiler_H
