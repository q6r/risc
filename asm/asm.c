#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "asm.h"
#include "emu.h"

int main(int argc, char **argv)
{

	FILE *fd;
	long fd_size;
	long fd_lines = 0;
	char *source_name, *output_name;

	if (argc < 3) {
		fprintf(stderr, "Usage : %s <file.asm> <output.bin>\n",
			argv[0]);
		return -1;
	}

	source_name = argv[1];
	output_name = argv[2];

	if ((fd = fopen(source_name, "rb")) == NULL) {
		perror("fopen");
		return -1;
	}

	fseek(fd, 0, SEEK_END);
	fd_size = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	printf("Source code : %s (%d bytes)\n", source_name, fd_size);
	printf("Compiled code : %s\n", output_name);

	// count lines in source code
	char c;
	while ((c = fgetc(fd)) != EOF)
		if (c == 0x0a)
			fd_lines++;
	fseek(fd, 0, SEEK_SET);

	printf("There are %d lines of code\n", fd_lines);

	// read file contents by lines into code_lines
	char **code_lines = { 0 };
	int cl_idx = 0;
	char *line = NULL;
	ssize_t read = 0;
	size_t len = 0;
	int i = 0;

	code_lines = (char **)malloc(sizeof(char *) * fd_lines + 1);
	if (code_lines == NULL) {
		perror("malloc");
		fclose(fd);
		return -1;
	}

	while ((read = getline(&line, &len, fd)) != -1) {
		code_lines[cl_idx] = (char *)malloc(sizeof(char) * read + 1);
		if (code_lines[cl_idx] == NULL) {
			perror("malloc");
			goto clean;
		}
		memcpy(code_lines[cl_idx], line, read);
		cl_idx++;
	}
	if (line != NULL)
		free(line);

	for (i = 0; i < fd_lines; i++) {
		printf("%s", code_lines[i]);
	}

	// XXX build binary insts 
	for (i = 0; i < fd_lines; i++) {
		binary_code_t cur_binc;
		if (convert_to_binarycode(&cur_binc, code_lines[i]) == false) {
			printf("Unable to translate %s\n", code_lines[i]);
		} else {
			printf("cur_binc.opcode = %.2x\n"
			       "cur_binc.reg    = %.2x\n"
			       "cur_binc.val    = %.8x\n"
			       "===========================\n",
			       cur_binc.opcode, cur_binc.reg, cur_binc.val);
		}
	}
	// XXX write binary instructions
	// XXX close and clean
	//
 clean:
	for (i = 0; i < fd_lines; i++)
		if (code_lines[i] != NULL)
			free(code_lines[i]);
	if (code_lines != NULL)
		free(code_lines);
	fclose(fd);
	return 0;
}

bool convert_to_binarycode(binary_code_t * res, char *line)
{
	int i;

	char *token = strtok(line, " ,\n");
	if (token == NULL) {
		fprintf(stderr, "No operator found!\n");
		return false;
	}
	// find operator and rules
	opcode_bundle_t opbundle;
	if (get_opcode_bundle(&opbundle, token) == false) {
		fprintf(stderr, "%s is invalid op", token);
		return false;
	}

	res->opcode = opbundle.opcode;

	// operator found. We extract the two parts of the
	// eg: op <part1>, <part2>
	// then we handle it depending on the extracted
	// operator flags

	// if anything than T_NOTHING then
	// we have a second value
	if (!(opbundle.flag & T_NOTHING)) {

		// get register or immu
		token = strtok(NULL, " ,\n");
		if (token == NULL) {
			printf("Invalid line found in : %s"
			       " (expected comma)\n", line);
			return false;
		}

		if (opbundle.flag & T_REGISTER /*  ... ? */ ) {
			res->reg = regstr_to_regbyte(token);
			if (res->reg == INVALID_REG) {
				printf("%s is an invalid register\n", token);
				return false;
			}

			if (opbundle.flag & T_IMMU) {
				token = strtok(NULL, " ,\n");
				if (token == NULL) {
					printf
					    ("Invalid code expected second part\n");
					return false;
				}
				// now we have a N possible solutions here
				// 1. if T_REGISTER **ONLY** only then it takes op r,r
				// and we need to use regstr_to_reg..
				// and set res->val.u8 // regsize
				// 2. if T_8 && !T_REGISTER then it takes op r, u8
				// and we set res->val.u8
				// 3. if T_16 && !T_REGISTER then it takes op r, u16
				// and we set res.val.u16
				// 4. if T_32 && !T_REGISTER then it takes op r, u32
				// and we set res.val.u16
			}
		} else {	// it only uses immu things like pushib, pushiw, pushid
			assert(opbundle.flag & T_IMMU
			       && !(opbundle.flag & T_REGISTER));
		}

		// for exit and sys_call and those with T_NOTHING
	} else {
		assert(!(opbundle.flag & T_IMMU && opbundle.flag & T_REGISTER));
		printf("syscall\n");
	}

	printf("Token : %s\n", token);
	printf("opbundle.name = %s\n", opbundle.name);
	printf("opbundle.opcode = %.8x\n", opbundle.opcode);
	printf("opbundle.flag = %.8x\n", opbundle.flag);
	if (opbundle.flag & T_REGISTER) {
		printf("This is a register\n");
		// then we take u8
	}
	if (opbundle.flag & T_IMMU) {
		printf("This is a immu\n");
	}

	if (opbundle.flag & T_NOTHING) {
		printf("This takes nothing\n");
	}

	res = NULL;
	return true;
}

u8 regstr_to_regbyte(char *reg)
{

	if (!strcasecmp(reg, "pc")) {
		return RPC;
	} else if (!strcasecmp(reg, "ps")) {
		return RPS;
	} else if (!strcasecmp(reg, "r1")) {
		return RR1;
	} else if (!strcasecmp(reg, "r2")) {
		return RR2;
	} else if (!strcasecmp(reg, "r3")) {
		return RR3;
	} else if (!strcasecmp(reg, "r4")) {
		return RR4;
	} else if (!strcasecmp(reg, "r5")) {
		return RR5;
	} else if (!strcasecmp(reg, "r6")) {
		return RR6;
	} else if (!strcasecmp(reg, "r7")) {
		return RR7;
	} else if (!strcasecmp(reg, "r8")) {
		return RR8;
	}

	return INVALID_REG;
}

bool get_opcode_bundle(opcode_bundle_t * res, char *opcode)
{
	int i, tran_table_sz;
	tran_table_sz =
	    sizeof(translation_table) / sizeof(translation_table[0]);
	for (i = 0; i < tran_table_sz; i++) {
		opcode_bundle_t curbundle = translation_table[i].ob;
		if (!strcasecmp(opcode, curbundle.name)) {
			strcpy(res->name, curbundle.name);
			res->opcode = curbundle.opcode;
			res->flag = curbundle.flag;
			return true;
		}
	}
	return false;
}
