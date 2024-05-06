#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILENAME "mips.txt"

// initialize register storages, following MIPS convention
char *register_filenames[32] = {"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7" , "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

char *r_type_list[7] = {"add", "sub", "and", "or", "slt", "move", "jr"};
char *i_type_list[8] = {"addi", "addiu", "beq", "bne", "sw", "lw", "lui", "ori"};
char *j_type_list[2] = {"j", "jal"};

void add_symboltable(FILE *stream, char *label, unsigned int address){
	fprintf(stream, "%s    0x%08x\n", label, address);
}

unsigned int register_imp(char *reg_input, int k, int move_check, int *reg_type){
    for(k = 0; k < 31; k++){
        if(strcmp(reg_input, register_filenames[k]) == 0){
            printf("k = %d\n", k);
            switch(*reg_type){
                case 0: // rd
                    return (k << 11);
                    *reg_type++;
                    break;
                case 1: // rs
                    return (k << 21);
                    *reg_type++;
                    break;
                case 2: // rt
                    return (k << 16);
                    *reg_type = 0;
                    break;
            }
            break;
        }
    }
}

void assembler_pass(int N, FILE *fp, FILE *sym_table, int pass_check) {
	char input_line[100];
	char newline_check;
	unsigned int pc = 0x00400000;
	unsigned int data_address = 0x10000000;
    unsigned int instruction_in_hex;
    int input_string_len;
    
    //boolean
    int r_type; // 0 = non r_type, 1 = r_type
    int i_type; // 0 = non r_type, 1 = r_type
    int j_type; // 0 = non r_type, 1 = r_type
    int label_inst; // 0 = no label in instruction, 1 = has label in instruction
	int inst_or_reg;  // 0 = instruction, 1 = register
	int segment_part; // 0 = part of text segment, 1 = start of data segment

    //tertiary boolean
    int register_type; // 0 = rd, 1 = rs, 2 = rt
    
    //for register parsing
    char *reg;
	
	int i = 0;
    int j = 0;
    int k = 0;
	while (i <= N) {
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
                        printf("--> register names: %s\n", input_line);
                        if(r_type){ // rs,rt,rd
                            if(j == 6){ //special case; why is jr an r type
                                printf("$jr, will implement\n");
                            }
                            else{
                                register_type = 0;
                                reg = strtok(input_line, ",");
                                while(reg != NULL){
                                    instruction_in_hex += register_imp(reg, k, j, &register_type);
                                    // printf("%s\n", reg);
                                    reg = strtok(NULL, ",");
                                    register_type++;
                                }
                            }
                            printf("--> machine code of current instruction: %08x\n", instruction_in_hex);
                        }
                        else if(i_type){ // rs,rt,imm
                            
                        }
                        else if(j_type){ // label
                            
                        }
                        else{
                            printf("there's a problem houston\n");
                        }
                    }
                }
                pc += 4;
            }
		}
		else if(newline_check == ' '){ // still part of an instruction (label, inst)
			input_string_len = strlen(input_line);
            r_type = 0;
            i_type = 0;
            j_type = 0;
            
			if(input_line[input_string_len - 1] == ':'){ // label found; add to symbol table
				input_line[input_string_len - 1] = '\0'; //remove colon from input
				if(pass_check){
                    printf("label \"%s\" with address: %08x\n", input_line, pc);
                    continue;
                }
                else{
                    add_symboltable(sym_table, input_line, pc);
                }
			}
            else if (pass_check){ //instruction found
                instruction_in_hex = 0;
                // r type check
                for(j = 0; j < 7; j++){
                    if(strcmp(input_line, r_type_list[j]) == 0){
                        r_type = 1;
                        switch(j){
                            case 0:
                                instruction_in_hex += 32;
                                break;
                            case 1:
                                instruction_in_hex += 34;
                                break;
                            case 2:
                                instruction_in_hex += 36;
                                break;
                            case 3:
                                instruction_in_hex += 37;
                                break;
                            case 4:
                                instruction_in_hex += 42;
                                break;
                            case 5: //psudoinstruction
                                instruction_in_hex += 32;
                                break;
                            case 6: 
                                instruction_in_hex += 8;
                                break;
                        }
                        printf("R instruction \'%s\' with address: %08x\n", input_line, pc);
                        continue;
                    }
                }

                // i type check
                for(int j = 0; j < 8; j++){
                    if(strcmp(input_line, i_type_list[j]) == 0){
                        printf("I instruction \'%s\' with address: %08x\n", input_line, pc);
                    }
                }

                // j type check
                for(int j = 0; j < 2; j++){
                    if(strcmp(input_line, j_type_list[j]) == 0){
                        printf("J instruction \'%s\' with address: %08x\n", input_line, pc);
                    }
                }                
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
