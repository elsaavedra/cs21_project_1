#include <stdio.h>

#ifndef _MACROS_H_
#define _MACROS_H_

char *print_str(FILE *exec, unsigned int *pc, unsigned int address);

char *read_str(FILE *exec, unsigned int *pc, unsigned int address, int max_bytes);

char *print_integer(FILE *exec, unsigned int *pc, int my_int);

char *read_integer(FILE *exec, unsigned int *pc);

char *exit_macro(FILE *exec, unsigned int *pc);

#endif
