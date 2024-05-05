#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILENAME "mips.txt"

typedef struct node NODE;
struct node {
    //found in all types
	unsigned int op;

    //r + i-type
	unsigned int rs;
	unsigned int rt;

    //r-type
	unsigned int rd;
    unsigned int shamt;
    unsigned int funct;

    //i-type
    unsigned int imm;
    //j-type
    unsigned int address;
};

// initialize register storages, following MIPS convention
char *register_filenames[32] = {"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7" , "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

void add_symboltable(FILE *stream, char *label, unsigned int address){
	fprintf(stream, "%s    0x%08x\n", label, address);
}

void assembler_pass(int N, FILE *fp, FILE *sym_table, int pass_check) {
	char input_line[100];
	char newline_check;
	unsigned int pc = 0x00400000;
	unsigned int data_address = 0x10000000;
	int input_string_len;
	int inst_or_reg;  // 0 = instruction, 1 = register
	int segment_part; // 0 = part of text segment, 1 = start of data segment
	
	int i = 0;
	while (i < N) {
		// scan each line and check if whitespace or newline
		fscanf(fp, "%s%c", input_line, &newline_check);
	
		// check for .text, .data, .include “macros.asm”
		if (strcmp(input_line, ".include") == 0) {
	  		fscanf(fp, "%s%c", input_line, &newline_check); // checks file name of macros
		} 
		else if (strcmp(input_line, ".text") == 0) {
	  		segment_part = 0;
	  		i++;
	  		continue; // skips address addition
		} 
		else if (strcmp(input_line, ".data") == 0) {
	  		segment_part = 1;
	  		i++;
	  		continue; // skips address addition
		}
	
		// increase iteration by 1 if newline is found; register values
		if (newline_check == '\n') {
	  		i++;
            if (segment_part) {
                // data instructions here
                data_address += 4;
            }
            else {
                if(pass_check){
                    if(strcmp(input_line, "syscall") == 0){ //edgecase: syscall has no registers
                        printf("syscall instruction with address: %08x\n", pc);
                    }
                    else{
                        printf("--> register names: %s \n", input_line);
                    }
                }
                pc += 4;
            }
		}
		else if(newline_check == ' '){ // still part of an instruction (label, inst)
			input_string_len = strlen(input_line);
			if(input_line[input_string_len - 1] == ':'){ // label found; add to symbol table
				input_line[input_string_len - 1] = '\0'; //remove colon from input
				if(pass_check){
                    printf("label \"%s\" with address: %08x\n", input_line, pc);
                }
                else{
                    add_symboltable(sym_table, input_line, pc);
                }
			}
            else if (pass_check){ //instruction found
                printf("instruction \'%s\' with address: %08x\n", input_line, pc);
            }
		}
		else{
			printf("how?\n");
		}
	}
}

int main(void) {
	// open input file using fopen; if no txt file found, mips
	FILE *fp = fopen(INPUT_FILENAME, "r");
	if (fp == NULL) {
		printf("Error: Cannot open or find filename %s", INPUT_FILENAME);
		exit(1);
	}

	// create / edit symboltable file
	FILE *sym_table = fopen("symboltable.txt", "w+");
	
	int lines;
	fscanf(fp, "%d", &lines);
	printf("%d\n", lines); //sanity check
	
	// get inputs line by line
	assembler_pass(lines, fp, sym_table, 0);
	printf("Assemble: First-pass complete...\n");
	fseek(fp, 0, SEEK_SET);
	assembler_pass(lines, fp, sym_table, 1);
	printf("Assemble: Second-pass complete...\n");
	return 0;
}
