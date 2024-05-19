# MIPS Assembler in C
An implementation of the two-pass assembler MIPS Architecture in C. To run the code, download `mips.txt` and `mips_assembler` and run `mips_assembler` using a C compiler. Edit `mips.txt` for a different output (check Limitations section).

## Current Implementation
As of **_May 20 2024_**, the following have been implemented:
- [x] Create a `symboltable.txt` containing the labels of instructions
- [x] Finish first pass of assembler
- [x] Implement R-Type instruction identifier
- [x] Implement I-Type instruction identifier
- [x] Implement J-Type instruction identifier
- [x] Implement pseudocode instruction identifier
- [x] Implement data segment of code
- [ ] Implement `macros.c`
- [ ] Finish second pass of assembler
- [ ] Execute MIPS code in console
- [ ] Polish final code for submission

## Macro file
To be implemented.

## Limitations
Each line in `mips.txt` is limited to 100 characters. To change this, replace 100 with another desired number in
```
#define CHAR_LIMIT 100
```
Do note that the set of instructions are limited to the following instructions:
```
add, sub, addi, addiu, and, or, slt, beq, bne, lw, sw, move, j, jal, lui, ori, syscall 
```
