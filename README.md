# MIPS Assembler in C
An implementation of the two-pass assembler MIPS Architecture in C. To run the code, download

## Current Implementation
As of **_May 06 2024_**, the following have been implemented:
- [x] Create a `symboltable.txt` containing the labels of instructions
- [x] Finish first pass of assembler
- [x] Implement R-Type instruction identifier
- [ ] Implement I-Type instruction identifier
- [ ] Implement J-Type instruction identifier
- [ ] Implement pseudocode instruction identifier
- [ ] Implement `macros.asm`
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
