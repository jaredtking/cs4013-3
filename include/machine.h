#ifndef MACHINE_H
#define MACHINE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "token.h"

#define MAX_ID_LEN 10
#define MAX_INT_LEN 10
#define MAX_REAL_XX_LEN 5
#define MAX_REAL_YY_LEN 5
#define MAX_REAL_ZZ_LEN 2

#define MACHINE_ERR_NONE 0
#define MACHINE_ERR_NO_MATCH 1
#define MACHINE_ERR_ID_TOO_LONG 2
#define MACHINE_ERR_INT_TOO_LONG 3
#define MACHINE_ERR_REAL_XX_TOO_LONG 4
#define MACHINE_ERR_REAL_YY_TOO_LONG 5
#define MACHINE_ERR_REAL_ZZ_TOO_LONG 6
#define MACHINE_ERR_NUM_LEADING_ZERO 7

MachineResult machine_omega(char *in, ReservedWord *reserved_words, SymbolTable *symbol_table);
MachineResult machine_whitespace(char *in);
MachineResult machine_idres(char *in, ReservedWord *reserved_words, SymbolTable *symbol_table);
MachineResult machine_int(char *in);
MachineResult machine_real(char *in);
MachineResult machine_longreal(char *in);
MachineResult machine_catchall(char *in);

int is_alpha(char c);
int is_alpha_numeric(char c);
int is_numeric(char c);
int is_plus_or_minus(char c);
ReservedWord *is_reserved_word(char *word, ReservedWord *reserved_words);
int is_whitespace(char c);
int get_sym_table_addr(char *word, SymbolTable *symbol_table, int loc);

#endif