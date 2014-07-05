// TODO : Implement syscall and exit translations.
#include "compiler.h"

#define CHECK_TOKEN_ERR(TOK, ERR, RET) \
do{if(TOK == NULL) { printf(ERR); return RET;}} while(0)
/*
asm_table_t asm_table[100] = {
     {OP_MOV, "mov", REG_REG, 3},
     {OP_ADD, "add", REG_REG, 3},
     {OP_SUB, "sub", REG_REG, 3},
     {OP_INC, "inc", REG, 2},
     {OP_DEC, "dec", REG, 2},
     {OP_MOVIB, "movib", REG_IMMB, 3},
     {OP_MOVIW, "moviw", REG_IMMW, 4},
     {OP_MOVID, "movid", REG_IMMD, 6},
     {OP_PUSHIB, "pushib", IMMB, 3},
     {OP_PUSHIW, "pushiw", IMMW, 4},
     {OP_PUSHID, "pushid", IMMD, 6},
     //.....etc
};
*/
asm_table_t asm_table[100] = {
    /* reg{OP_reg_t}, reg{OP_reg_t} and reg{OP_reg_t} operators */
     {OP_ADD, "add", REG_REG},
     {OP_SUB, "sub", REG_REG},
     {OP_MUL, "mul", REG_REG},
     {OP_DIV, "div", REG_REG},
     {OP_XOR, "xor", REG_REG},
     {OP_MOV, "mov", REG_REG},
     {OP_INC, "inc", REG_REG},
     {OP_DEC, "dec", REG_REG},
     {OP_PUSHB, "pushb", REG_IMMB},
     {OP_PUSHW, "pushw", REG_IMMW},
     {OP_PUSHD, "pushd", REG_IMMD},
     {OP_POPB, "popb", REG_IMMB},
     {OP_POPW, "popw", REG_IMMW},
     {OP_POPD, "popd", REG_IMMD},
     /* reg, immu{OP_32,16,8} and immu{OP_32,16,8} operators */
     // immb 8bit
     {OP_ADDIB, "addib", REG_IMMB},
     {OP_SUBIB, "subib", REG_IMMB},
     {OP_MULIB, "mulib", REG_IMMB},
     {OP_DIVIB, "divib", REG_IMMB},
     {OP_XORIB, "xorib", REG_IMMB},
     {OP_MOVIB, "movib", REG_IMMB},
     {OP_PUSHIB, "pushib", IMMB},
     // immw 16bit
     {OP_ADDIW, "addiw", REG_IMMW},
     {OP_SUBIW, "subiw", REG_IMMW},
     {OP_MULIW, "muliw", REG_IMMW},
     {OP_DIVIW, "diviw", REG_IMMW},
     {OP_XORIW, "xoriw", REG_IMMW},
     {OP_MOVIW, "moviw", REG_IMMW},
     {OP_PUSHIW, "pushiw", IMMW},
     // immd 32bit
     {OP_ADDID, "addid", REG_IMMD},
     {OP_SUBID, "subid", REG_IMMD},
     {OP_MULID, "mulid", REG_IMMD},
     {OP_DIVID, "divid", REG_IMMD},
     {OP_XORID, "xorid", REG_IMMD},
     {OP_MOVID, "movid", REG_IMMD},
     {OP_PUSHID, "pushid", IMMD},
     /* others */
     {OP_GSTK, "gstk", REG},
     {OP_PSTK, "pstk", REG},
     //{OP_EXIT, "exit", REG},
     //{OP_SYSCALL, "sys_call", REG}
};

u8 *read_file_content(char *file_name) {
    // int fd = fopen(file_name, "r");
    // .... res = file_content;
    // fclose(fd);
    // return res;
    return NULL;
}

bool assemble(u8 *file_content) {
    char **lines = NULL;
    char *line   = NULL;
    int i;

    // extract lines in asm code
    lines = split_str(file_content, ";\n");
    if(lines == NULL) {
        fprintf(stderr, "split_str err\n");
        return false;
    }

    // Translate line by line
    for(i=0;lines[i] != NULL;i++) {
        line = lines[i];
        //printf("%s\n", line);
        char *translation = translate_line(line);
        if(translation == NULL) {
            fprintf(stderr, "translate_line : unable to translate\n");
            return false;
        }
        printf("%s", translation);
        free(line);
        free(translation);
    }

    return true;
}

#define MAX_TRANSLATION 256
char *translate_line(char *line) {
    int i;
    const char token_rules[] = " ,;\n";
    char *token = strtok(line, token_rules);
    char *translated_bytes = calloc(MAX_TRANSLATION, sizeof(char));
    if(translated_bytes == NULL) {
        perror("calloc");
        return NULL;
    }

    // walk the line parts and translate to asm_table_t *
    asm_table_t *op = NULL;

    while(token != NULL) {
        //printf("part : %s\n", token);
        // get op
        if(op == NULL) {
            op = get_op_from_table(token);
            //printf("op token -> = %s\n", token);
            if(op == NULL) {
                printf("Invalid op\n");
                free(translated_bytes);
                return NULL;
            }

            switch(op->op_type) {
            case REG:
               {
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u8 reg_a = reg_str_to_byte(token);
                  //printf("Translated to %.2x%.2x\n", op->op, reg_a);
                  sprintf(translated_bytes, "%.2x%.2x", op->op, reg_a);
               }
              break;
            case REG_REG:
               {
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u8 reg_a = reg_str_to_byte(token);
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u8 reg_b = reg_str_to_byte(token);
                  //printf("Translated to %.2x%.2x%.2x\n", op->op, reg_a, reg_b);
                  sprintf(translated_bytes, "%s%.2x%.2x%.2x", translated_bytes, op->op, reg_a, reg_b);
               }
              break;
            case REG_IMMB:
               {
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u8 reg_a = reg_str_to_byte(token);
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u8 immb = immb_str_to_byte(token);
                  //printf("Translated to %.2x%.2x%.2x\n", op->op, reg_a, immb);
                  sprintf(translated_bytes, "%s%.2x%.2x%.2x", translated_bytes, op->op, reg_a, immb);
               }
              break;
            case REG_IMMW:
               {
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u8 reg_a = reg_str_to_byte(token);
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u16 immw = immw_str_to_byte(token);
                  //printf("Translated to %.2x%.2x%.4x\n", op->op, reg_a, immw);
                  sprintf(translated_bytes, "%s%.2x%.2x%.4x", translated_bytes, op->op, reg_a, immw);
               }
              break;
            case REG_IMMD:
               {
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u8 reg_a = reg_str_to_byte(token);
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u32 immd = immd_str_to_byte(token);
                  //printf("Translated to %.2x%.2x%.8x\n", op->op, reg_a, immd);
                  sprintf(translated_bytes, "%s%.2x%.2x%.8x", translated_bytes, op->op, reg_a, immd);
               }
              break;
            case IMMB:
               {
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u8 immb = immb_str_to_byte(token);
                  //printf("Translated to %.2x%.2x\n", op->op, immb);
                  sprintf(translated_bytes, "%s%.2x%.2x", translated_bytes, op->op, immb);
               }
              break;
            case IMMW:
               {
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u16 immw = immw_str_to_byte(token);
                  //printf("Translated to %.2x%.4x\n", op->op, immw);
                  sprintf(translated_bytes, "%s%.2x%.4x", translated_bytes, op->op, immw);
               }
              break;
            case IMMD:
               {
                  token = strtok(NULL, token_rules);
                  CHECK_TOKEN_ERR(token, "error", NULL);
                  u32 immd = immd_str_to_byte(token);
                  //printf("Translated to %.2x%.8x\n", op->op, immd);
                  sprintf(translated_bytes, "%s%.2x%.8x", translated_bytes, op->op, immd);
               }
              break;
            default:
               {
               }
              break;
            };

            free(op);
            op = NULL;
        }


        token = strtok(NULL, token_rules);
    }

    return translated_bytes;
}

// TODO : handle converting failures
u8 immb_str_to_byte(const char *str) {
    u8 res = 0;
    const char *str_org = str;
    if(sscanf(str, "%x", &res) <= 0) {
        printf("Unable to convert %s to hex!\n", str);
    }
    str = str_org; // sscanf! WTF
    return res;
}

// TODO : handle converting failures
u16 immw_str_to_byte(const char *str) {
    u16 res = 0;
    const char *str_org = str;
    if(sscanf(str, "%x", &res) <= 0) {
        printf("Unable to convert %s to hex!\n", str);
    }
    str = str_org;
    return res;
}

// TODO : handle converting failures
u32 immd_str_to_byte(const char *str) {
    u32 res = 0;
    const char *str_org = str;
    if(sscanf(str, "%x", &res) <= 0) {
        printf("Unable to convert %s to hex!\n", str);
    }
    str = str_org;
    return res;
}

asm_table_t *get_op_from_table(char *opname) {
    int i;
    asm_table_t *res = NULL;

    for(i=0;i<100;i++) {
        if(!strcasecmp(asm_table[i].op_name, opname)) {
            res = calloc(1, sizeof(asm_table));
            res->op = asm_table[i].op;
            strcpy(res->op_name, asm_table[i].op_name);
            res->op_type = asm_table[i].op_type;
            //res->max = asm_table[i].max;
            return res;
        }
    }

    return NULL;
}
u8 *compile(char *file_name) {
    // u8 *file_content = read_file_content(filename);
    // u8 *code = assemble(file_content);
    // free(file_content);
    // return translated_opcodes;
    return NULL;
}

char **split_str(char *content, const char *delim) {
    char *tmp_content = content;
    size_t count      = 0;
    char **res        = NULL;
    size_t i = 0;

    if(content == NULL) {
        return NULL;
    }

    // count delims
    while(*tmp_content) {
        for(i=0;i<strlen(delim);i++) {
            if(delim[i] == *tmp_content) {
                count++;
            }
        }
        tmp_content++;
    }

    count++;
    res = calloc(count, sizeof(char *));
    if(res == NULL) {
        perror("calloc");
        return NULL;
    }


    //const char *pdelim = &delim;
    char *token = strtok(content, delim);
    i=0;
    while(token) {
        res[i++] = strdup(token);
        token = strtok(NULL, delim);
    }
    res[i] = NULL;

    return res;
}

u8 reg_str_to_byte(const char *str) {
    u8 *reg_s[] = {"r1","r2","r3", "r4", "r5", "r6", "r7", "r8", "ps", "pc" };
    u8 reg_b[] = {RR1, RR2, RR3, RR4, RR5, RR6, RR7, RR8, RPS, RPC };
    size_t i;
    for(i=0;i<sizeof(reg_s)/sizeof(reg_s[0]);i++) {
        if(!strcasecmp(reg_s[i], str))
            return reg_b[i];
    }
    return -1;
}
