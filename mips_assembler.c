// call macros C file
#include "macros.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILENAME "mips.txt"
#define CHAR_LIMIT 100

// initialize register storages, following MIPS convention
char *register_filenames[32] = {"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7" , "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

char *r_type_list[7] = {"add", "sub", "and", "or", "slt", "move", "jr"};
char *i_type_list[8] = {"addi", "addiu", "beq", "bne", "sw", "lw", "lui", "ori"};
char *j_type_list[2] = {"j", "jal"};
char *pseudo_list[2] = {"li", "la"};
char *macro_list[5] = {"print_str", "read_str", "print_integer", "read_integer", "exit"};

void add_symboltable(FILE *stream, char *label, unsigned int address){
    fprintf(stream, "%s    0x%08x\n", label, address);
}

unsigned int label_hunting(FILE *sym_table, char *label){
    char temp_label[CHAR_LIMIT];
    unsigned int temp_address;

    fseek(sym_table, 0, SEEK_SET);
    fscanf(sym_table, "%s    0x%08x\n", temp_label, &temp_address);
    while(strcmp(label, temp_label)!= 0){
        fscanf(sym_table, "%s    0x%08x", temp_label, &temp_address);
        // ends when a label is found; will ALWAYS terminate
    }
    // printf("---->labels found: %s with inst address %08x\n", temp_label, temp_address);
    
    return temp_address;
}

unsigned int register_imp_r(char *reg_input, int k, int j, int *reg_type){    
    for(k = 0; k < 32; k++){
        if(strcmp(reg_input, register_filenames[k]) == 0){
            switch(*reg_type){
                case 0: // rd                    
                    return (k << 11);
                    break;
                case 1: // rs
                    return (k << 21);
                    break;
                case 2: // rt
                    return (k << 16);
                    break;
            }
            break;
        }
    }
    return 0;
}

unsigned int register_imp_i(char *reg, int k, int j, FILE *sym_table, int char_type, int pc){
    int reg_type; // 0 = rs, 1 = rt, 2 = imm, 3 = label
    int imm = 0;
    int label_addr = 0;
    int start = 0;
    int len = strlen(reg);
    char reg_parsed[100];
    char *memory;
    
    if (j == 0 || j == 1 || j == 7){ // rs, rt, imm
        switch(char_type){
            case 0:
                reg_type = 0;
                break;
            case 1:
                reg_type = 1;
                break;
            case 2:
                imm = strtol(reg, NULL, 10);
                reg_type = 2;
                break;
        }
    }
    else if (j == 2 || j == 3){ // rs, rt, label
        switch(char_type){
            case 0:
                reg_type = 0;
                break;
            case 1:
                reg_type = 1;
                break;
            case 2:
                reg_type = 3;
                break;
        }
    }
    else if (j == 4){ // rt, imm(rs); WARNING: lw rs, label IS NOT IMPLEMENTED
        switch(char_type){
            case 0:
                reg_type = 1;
                break;
            case 1:
                imm = strtol(reg, &memory, 10);
                reg_type = 0;
                for(int i = 0; i < len; i++){
                    if(start){
                        if(reg[i] == ')'){
                            reg_parsed[start-1] = '\0';
                            break;
                        }
                        reg_parsed[start-1] = reg[i];
                        start++;
                    }
                    else if(reg[i] == '('){
                        start++;
                    }
                }
                // printf("%s", reg_parsed);
                // printf("%d", !strcmp(reg_parsed, "$sp"));
                break;
        }
    }
    else if (j == 5){ // rs, imm(rt); WARNING: lw rs, label IS NOT IMPLEMENTED
        switch(char_type){
            case 0:
                reg_type = 0;
                break;
            case 1:
                imm = strtol(reg, &memory, 10);
                reg_type = 1;
                for(int i = 0; i < len; i++){
                    if(start){
                        if(reg[i] == ')'){
                            reg_parsed[start-1] = '\0';
                            break;
                        }
                        reg_parsed[start-1] = reg[i];
                        start++;
                    }
                    else if(reg[i] == '('){
                        start++;
                    }
                }
                break;
        }
    }
    else if (j == 6){ // rt, imm 
        switch(char_type){
            case 0:
                reg_type = 1;
                break;
            case 1:
                imm = strtol(reg, NULL, 10);
                reg_type = 2;
                break;
        }
    }

    //for rs and rt
    if (reg_type == 0 || reg_type == 1){
        for(k = 0; k < 32; k++){
            if(j == 5 && !strcmp(reg_parsed, register_filenames[k]) && reg_type == 1){ // lw
                // imm
                if (reg[0] == '-'){
                    imm = (imm ^ (1 << 15)) + 1;
                    // printf("%d\n", imm);
                }
                
                return (k << 16) + imm;
            }
            else if(j == 4 && !strcmp(reg_parsed, register_filenames[k]) && reg_type == 0){ // sw
                // imm
                if (reg[0] == '-'){
                    imm = (imm ^ (1 << 15)) + 1;
                    // printf("%d\n", imm);
                }
                
                return (k << 21) + imm;
            }
            else if(!strcmp(reg, register_filenames[k]) && reg_type == 1){ // general
                return (k << 16);
            }
            else if(j == 6){ break; }
            else if(!strcmp(reg, register_filenames[k]) && reg_type == 0){ // general
                return (k << 21);
            }
        }
    }

    // check if immediate is negative (for 2C)
    if (reg_type == 3){ // label exists; do not add immediate
        label_addr = label_hunting(sym_table, reg);
        return ((label_addr - pc - 4) / 4);
    }
    if (reg[0] == '-'){
        imm = (imm ^ (1 << 15)) + 1;
        // printf("%d\n", imm);
        return imm;
    }
    else if (imm >= 0 && imm < 65536){
        return imm;
    }
    return 0;
}

void r_type_check(int *j, char *input, unsigned int *machine_code, int *type_check, int *cont){
    if(*cont){
        return;
    }
    
    for(*j = 0; *j < 7; (*j)++){
        if(strcmp(input, r_type_list[*j]) == 0){
            *type_check = 1;
            *cont = 1;
            switch(*j){
                case 0: // add
                    *machine_code += 32;
                    break;
                case 1: // sub
                    *machine_code += 34;
                    break;
                case 2: // and
                    *machine_code += 36;
                    break;
                case 3: // or
                    *machine_code += 37;
                    break;
                case 4: // slt
                    *machine_code += 42;
                    break;
                case 5: // psudoinstruction; move
                    *machine_code += 32;
                    break;
                case 6: // jr
                    *machine_code += 8;
                    break;
            }
            printf("R instruction \'%s\' with address: ", input);
            break;
        }
    }
}

void i_type_check(int *j, char *input, unsigned int *machine_code, int *type_check, int *cont){
    if(*cont){
        return;
    }
    
    int var = 0;
    for(*j = 0; *j < 8; (*j)++){
        if(strcmp(input, i_type_list[*j]) == 0){
            *type_check = 1;
            *cont = 1;
            switch(*j){
                case 0: // addi
                    var = 8;
                    break;
                case 1: // addiu
                    var = 9;
                    break;
                case 2: // beq
                    var = 4;
                    break;
                case 3: // bne
                    var = 5;
                    break;
                case 4: // sw
                    var = 43;
                    break;
                case 5: // lw
                    var = 35;
                    break; 
                case 6: // lui
                    var = 15;
                    break;
                case 7: // ori
                    var = 13;
                    break;
            }
            *machine_code += (var << 26); // opcode
            printf("I instruction \'%s\' with address: ", input);
            break;
        }
    }
}

void j_type_check(int *j, char *input, unsigned int *machine_code, int *type_check, int *cont){
    if(*cont){
        return;
    }
    
    int var;
    for(*j = 0; *j < 2; (*j)++){
        if(strcmp(input, j_type_list[*j]) == 0){
            *type_check = 1;
            *cont = 1;
            switch(*j){ // j
                case 0:
                    var = 2;
                    break;
                case 1: // jal
                    var = 3;
                    break;
            }
            *machine_code += (var << 26); // opcode
            printf("J instruction \'%s\' with address: ", input);
            break;
        }
    }
}

void assembler_pass(int N, FILE *fp, FILE *sym_table, int pass_check) {
    // general variables
    char input_line[CHAR_LIMIT];
    char newline_check;
    unsigned int pc = 0x00400000;
    unsigned int data_address = 0x10000000;
    unsigned int instruction_in_hex;
    int input_string_len;

    // boolean
    int r_type; // 0 = non r_type, 1 = r_type
    int i_type; // 0 = non i_type, 1 = i_type
    int j_type; // 0 = non j_type, 1 = j_type
    int pseudo; // 0 = non pseudo, 1 = psudo
    int label_inst; // 0 = no label in instruction, 1 = has label in instruction
    int inst_or_reg;  // 0 = instruction, 1 = register
    int segment_part = 0; // 0 = part of text segment, 1 = start of data segment
    int continue_check; // 0 = do not continue, 1 = continue
    int macros = 0;

    // tertiary boolean
    int register_type; // 0 = rd, 1 = rs, 2 = rt

    // for register parsing
    char *reg;

    // for pseudoinstruction parsing
    unsigned int temp_imm = 0;
    unsigned int upper_imm = 0;
    char *temp_reg;
    char temp_label[CHAR_LIMIT];

    // for macros parsing
    char macro_label[CHAR_LIMIT];
    
    // iterations
    int i = 0;
    int j = 0;
    int k = 0;
    
    while (i < N) {
        // scan each line and check if whitespace or newline
        fscanf(fp, "%s%c", input_line, &newline_check);

        // check for .text, .data, .include “macros.asm”
        if (strcmp(input_line, ".include") == 0) {
            fscanf(fp, "%s%c", input_line, &newline_check); // checks file name of macros
            macros = 1; // for convention
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
                    strcpy(macro_label, input_line);
                    strtok(macro_label, "(");
                    
                    if(strcmp(input_line, "syscall") == 0){ // edgecase: syscall has no registers
                        printf("syscall instruction with address: %08x\n", pc);
                        instruction_in_hex = 0;
                        printf("--> machine code of current instruction: %08x\n", instruction_in_hex);
                    }
                    else if(strcmp(input_line, macro_label) && i_type == 0){ // macro is found if the condition is non-zero
                        printf("macro instruction (\"%s\") with address: %08x\n", macro_label, pc);
                        // macro pulling from macros.c
                        if(strcmp(macro_list[0], macro_label) == 0){
                            printf("%s", print_str());
                        }
                        else if(strcmp(macro_list[1], macro_label) == 0){
                            printf("%s", read_str());
                        }
                        else if(strcmp(macro_list[2], macro_label) == 0){
                            printf("%s", print_integer());
                        }
                        else if(strcmp(macro_list[3], macro_label) == 0){
                            printf("%s", read_integer());
                        }
                        else if(strcmp(macro_list[4], macro_label) == 0){
                            printf("%s", exit_macro());
                        }
                    }
                    else{
                        // printf("--> register names: %s\n", input_line);
                        instruction_in_hex = 0;
                        if(r_type){ // rs,rt,rd
                            if(j == 6){ //special case; why is jr an r type
                                register_type = 1;
                                instruction_in_hex += register_imp_r(input_line, k, j, &register_type);
                            }
                            else{
                                register_type = 0;
                                reg = strtok(input_line, ",");
                                while(reg != NULL){
                                    instruction_in_hex += register_imp_r(reg, k, j, &register_type);
                                    // printf("%s\n", reg);
                                    reg = strtok(NULL, ",");
                                    register_type++;
                                }
                            }
                            printf("--> machine code of current instruction: %08x\n", instruction_in_hex);
                        }
                        else if(i_type){ // rs,rt,imm
                            register_type = 0;
                            reg = strtok(input_line, ",");
                            while(reg != NULL){
                                instruction_in_hex += register_imp_i(reg, k, j, sym_table, register_type, pc);
                                // printf("%s\n", reg);
                                reg = strtok(NULL, ",");
                                register_type++;
                            }
                            printf("--> machine code of current instruction: %08x\n", instruction_in_hex);
                        }
                        else if(j_type){ // label
                            if(macros && strcmp(input_line, "GCD") == 0){ // special case for GCD: requires jal
                                instruction_in_hex += 0x0C004000;
                            }
                            else{
                                unsigned int non_parsed_addr = label_hunting(sym_table, input_line);
                                non_parsed_addr = non_parsed_addr & 0x0FFFFFFF; // bit masking
                                non_parsed_addr = non_parsed_addr >> 2;
                                instruction_in_hex += non_parsed_addr;
                            }                            
                            printf("--> machine code of current instruction: %08x\n", instruction_in_hex);
                        }
                        else if(pseudo){ // dependent on what pseudocode
                            if(pseudo == 1){ // li
                                register_type = 0;
                                reg = strtok(input_line, ",");
                                while(reg != NULL){
                                    if(reg[0] == '$'){ // register to be stored
                                        temp_reg = reg;
                                    }
                                    else { // immediate in decimal
                                         sscanf(reg, "%d", &temp_imm);
                                         if(temp_imm < 65536){ // decimals less than 65536 will have lui as 0x0
                                            instruction_in_hex = 0x3C010000;
                                            printf("--> machine code of current instruction: %08x\n", instruction_in_hex);
                                            pc += 4; //start ori inst
                                             
                                            instruction_in_hex = 0;
                                            instruction_in_hex += temp_imm; // no need for bit masking
                                         }
                                        else{
                                            instruction_in_hex = 0x3C010000;
                                            instruction_in_hex += (temp_imm - 65535);
                                            printf("--> machine code of current instruction: %08x\n", instruction_in_hex);
                                            pc += 4; //start ori inst
                                            
                                            instruction_in_hex = 0;
                                            instruction_in_hex += temp_imm & 0x0000FFFF; // bit mask for lower 16 bits only
                                        }

                                        i_type_check(&j, "ori", &instruction_in_hex, &i_type, &continue_check);
                                        instruction_in_hex += (1 << 21);

                                        for(k = 0; k < 32; k++){
                                            if(strcmp(register_filenames[k], temp_reg) == 0){
                                                instruction_in_hex += (k << 16);
                                                break;
                                            }
                                        }
                                        
                                        printf("%08x\n--> machine code of current instruction: %08x\n", pc, instruction_in_hex);
                                    }
                                    reg = strtok(NULL, ",");
                                    register_type++;
                                }
                            }
                            if(pseudo == 2){ // la
                                register_type = 0;
                                reg = strtok(input_line, ",");
                                while(reg != NULL){
                                    if(reg[0] == '$'){ // register to be stored
                                        temp_reg = reg;
                                    }
                                    else { // label
                                        sscanf(reg, "%s", temp_label);
                                        temp_imm = label_hunting(sym_table, temp_label);
                                        upper_imm = temp_imm >> 16;
                                        
                                        instruction_in_hex = 0x3C010000; // $at added
                                        instruction_in_hex += upper_imm;
                                        printf("--> machine code of current instruction: %08x\n", instruction_in_hex);
                                        pc += 4; //start ori inst
                                        instruction_in_hex = 0;
                                        instruction_in_hex += temp_imm & 0x0000FFFF; // bit mask for lower 16 bits only
                                        
                                        i_type_check(&j, "ori", &instruction_in_hex, &i_type, &continue_check);
                                        instruction_in_hex += (1 << 21);

                                        for(k = 0; k < 32; k++){
                                            if(strcmp(register_filenames[k], temp_reg) == 0){
                                                instruction_in_hex += (k << 16);
                                                break;
                                            }
                                        }

                                        printf("%08x\n--> machine code of current instruction: %08x\n", pc, instruction_in_hex);
                                    }
                                    reg = strtok(NULL, ",");
                                    register_type++;
                                }
                            }
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
                input_line[input_string_len - 1] = '\0'; // remove colon from input
                if(pass_check){
                    // printf("label \"%s\" with address: %08x\n", input_line, pc);
                    continue;
                }
                else{
                    add_symboltable(sym_table, input_line, pc);
                }
            }
            else if (pass_check){ //instruction found
                instruction_in_hex = 0;
                continue_check = 0;
                
                // r type check
                r_type_check(&j, input_line, &instruction_in_hex, &r_type, &continue_check);

                // i type check
                i_type_check(&j, input_line, &instruction_in_hex, &i_type, &continue_check);

                // j type check
                j_type_check(&j, input_line, &instruction_in_hex, &j_type, &continue_check);

                if (continue_check == 0){
                    //PSEUDO ALERT
                    for(k = 0; k < 2; k++){
                        if(strcmp(input_line, pseudo_list[k]) == 0){
                            switch(k){
                                case 0: // li                                  
                                    pseudo = 1;
                                    break;
                                case 1: // la
                                    pseudo = 2;
                                    break;
                            }
                            break;
                        }
                    }
                    printf("pseudoinstruction \'%s\' with address: ", input_line);
                }
                printf("%08x\n", pc);
            }
            else{ //pseudoinstruction check
                for(k = 0; k < 2; k++){
                    if(strcmp(input_line, pseudo_list[k]) == 0){
                        pc += 4; //all current pseudoinstructions are two layered, hence pc + 4
                        break;
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

    // create symboltable file
    FILE *sym_table = fopen("symboltable.txt", "w+");

    int lines;
    fscanf(fp, "%d", &lines);
    printf("%d\n", lines); //sanity check

    // first pass start
    assembler_pass(lines, fp, sym_table, 0);
    printf("Assemble: First-pass complete...\n");
    
    // second pass start
    fseek(fp, 0, SEEK_SET);
    fscanf(fp, "%d", &lines);
    assembler_pass(lines, fp, sym_table, 1);
    printf("Assemble: Second-pass in progress...\n");
    return 0;
}
