#include <stdio.h>
#include "macros.h"

char *print_str(FILE *exec, unsigned int *pc, unsigned int address){ 
    unsigned int upper_imm = address & 0xFFFF0000;
    unsigned int lower_imm = address & 0x0000FFFF;
    unsigned int instruction;

    // lui $at, upper_imm
    instruction = 0x3C010000 + upper_imm;
    printf("I instruction \'lui\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // ori $a0, $at, lower_imm
    instruction = 0x34240000 + lower_imm;
    printf("I instruction \'ori\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // lui $at, 0
    instruction = 0x3C010000;
    printf("I instruction \'lui\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // ori $a0, $at, 4
    instruction = 0x34220000 + 4;
    printf("I instruction \'ori\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // syscall
    instruction = 0;
    printf("syscall instruction with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    return "END OF print_str MACRO\n";
}

char *read_str(FILE *exec, unsigned int *pc, unsigned int address, int max_bytes){ // int *pc, char *label, int len
    unsigned int upper_imm = address & 0xFFFF0000;
    unsigned int lower_imm = address & 0x0000FFFF;
    unsigned int instruction;
    // lui $at, upper_imm
    instruction = 0x3C010000 + upper_imm;
    printf("I instruction \'lui\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // ori $a0, $at, lower_imm
    instruction = 0x34240000 + lower_imm;
    printf("I instruction \'ori\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // lui $at, 0
    instruction = 0x3C010000;
    printf("I instruction \'lui\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // ori $a1, $at, max_bytes
    instruction = 0x34250000 + max_bytes;
    printf("I instruction \'ori\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // lui $at, 0
    instruction = 0x3C010000;
    printf("I instruction \'lui\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // ori $v0, $at, 8
    instruction = 0x34220000 + 8;
    printf("I instruction \'ori\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // syscall
    instruction = 0;
    printf("syscall instruction with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    return "END OF read_str MACRO\n";
}

char *print_integer(FILE *exec, unsigned int *pc, int my_int){ // int *pc, char *label
    unsigned int upper_imm = my_int & 0xFFFF0000;
    unsigned int lower_imm = my_int & 0x0000FFFF;
    unsigned int instruction;
    // lui $at, upper_imm
    instruction = 0x3C010000 + upper_imm;
    printf("I instruction \'lui\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // ori $a0, $at, max_bytes
    instruction = 0x34220000 + lower_imm;
    printf("I instruction \'ori\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // lui $at, 0
    instruction = 0x3C010000;
    printf("I instruction \'lui\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // ori $v0, $at, 5
    instruction = 0x34220000 + 1;
    printf("I instruction \'ori\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // syscall
    instruction = 0;
    printf("syscall instruction with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    return "END OF print_integer MACRO\n";
}

char *read_integer(FILE *exec, unsigned int *pc){
    unsigned int instruction;
    // lui $at, 0
    instruction = 0x3C010000;
    printf("I instruction \'lui\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // ori $v0, $at, 5
    instruction = 0x34220000 + 5;
    printf("I instruction \'ori\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // syscall
    instruction = 0;
    printf("syscall instruction with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    return "END OF read_integer MACRO\n";
}

char *exit_macro(FILE *exec, unsigned int *pc){
    unsigned int instruction;
    // lui $at, 0
    instruction = 0x3C010000;
    printf("I instruction \'lui\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // ori $v0, $at, 10
    instruction = 0x34220000 + 10;
    printf("I instruction \'ori\' with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    *pc += 4;
    // syscall
    instruction = 0;
    printf("syscall instruction with address: %08x\n", *pc);
    printf("--> machine code of current instruction: %08x\n", instruction);
    return "END OF exit_macro MACRO\n";
}
