#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// initialize register storages, following MIPS convention
char *register_filenames[32] = {"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7" , "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
long long int register_files[32];

void data_scraping(int N, FILE *fp) {
	char input_line[100];
	char newline_check;
	long long int pc = 0x00400000;
	long long int data_address = 0x10000000;
	int input_string_len;
	int inst_or_reg;  // 0 = instruction, 1 = register
	int segment_part; // 0 = part of text segment, 1 = start of data segment
	
	int i = 0;
	while (i < N) {
		// scan each line and check if whitespace or newline
		fscanf(fp, "%s%c", input_line, &newline_check);
	
		// check for .text, .data, .include “macros.asm”
		if (strcmp(input_line, ".include") == 0) {
	  		printf("macros found; continue\n");
	  		fscanf(fp, "%s%c", input_line, &newline_check); // checks file name of macros
		} 
		else if (strcmp(input_line, ".text") == 0) {
	  		printf("text segment; continue\n");
	  		segment_part = 0;
	  		i++;
	  		continue; // skips address addition
		} 
		else if (strcmp(input_line, ".data") == 0) {
	  		printf("data segment; continue\n");
	  		segment_part = 1;
	  		i++;
	  		continue; // skips address addition
		}
	
		// increase iteration by 1 if newline is found; register values
		if (newline_check == '\n' || newline_check == EOF) {
	  		i++;
	  		if (segment_part) {
				data_address += 4;
	  		}
			else {
				if(strcmp(input_line, "syscall") == 0){ //edgecase: syscall has no registers
					printf("syscall instruction with address: %llx\n", pc);
				}
				else{
					printf("--> register names: %s \n", input_line);
				}
				pc += 4;
	  		}
		}
		else if(newline_check == ' '){ // still part of an instruction (label, inst)
			input_string_len = strlen(input_line);
			if(input_line[input_string_len - 1] == ':'){ // label found
				printf("label \"%s\" with address: %llx\n", input_line, pc);
			}
			else {	//instruction found
				printf("instruction \'%s\' with address: %llx\n", input_line, pc);
			}
		}
		else{
			printf("how?\n");
		}
	}
}

int main(void) {
	//initialize $gp and $sp
	register_files[29] = 0x7FFFFFFC;
	register_files[28] = 0x10008000;
	
	// open input file using fopen; if no txt file found, mips
	FILE *fp = fopen("mips.txt", "r");
	if (fp == NULL) {
		printf("Error: Cannot open or find filename %s", "mips.txt");
		exit(1);
	}
	
	int lines;
	fscanf(fp, "%d", &lines);
	printf("%d\n", lines); //sanity check
	
	// get inputs line by line
	data_scraping(lines, fp);
	return 0;
}