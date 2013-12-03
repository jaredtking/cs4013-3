#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct SymbolTable
{
	char *symbol;
	struct SymbolTable *next;
	struct SymbolTable *prev;
	struct SymbolTable *parent;
	struct SymbolTable *child;
} SymbolTable;

int get_sym_table_addr(char *word, SymbolTable *symbol_table, int loc);
void get_sym_type(char *name, SymbolTable *symbol_table);
void get_sym_num_params(char *name, SymbolTable *symbol_table);
void get_sym_ret_value(char *name, SymbolTable *symbol_table);
void fprint_symbol_table(FILE *f, SymbolTable *symbol_table);

#endif