#include "compiler.h"

u8 *read_file_content(char *file_name) {
  // int fd = fopen(file_name, "r");
  // .... res = file_content;
  // fclose(fd);
  // return res;
  return NULL;
}

u8 *assemble(u8 *file_content) {
    // Read opcode (if not opcode then exit)
    // Map the opcode with a structure that contains the rules
    // {
    //    MOV : reg_reg { 1:opcode + 2:regs = 3 }
    //    MOVID : reg_immd { 1:opcode + 1:reg + 4:bytes = 6 }
    //    MOVIW : reg_immw { 1:opcode + 1:reg + 2:bytes = 4 }
    //    MOVIB : reg_immb { 1:opcode + 1:reg + 1:bytes = 3 }
    //    ....etc
    // }
    //
    // ...etc
    return NULL;
}

u8 *compile(char *file_name) {
  // u8 *file_content = read_file_content(filename);
  // u8 *code = assemble(file_content);
  // free(file_content);
  // return translated_opcodes;
  return NULL;
}

