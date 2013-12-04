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

typedef enum Type
{
	PGNAME,
	FNAME,
	INT,
	REAL
} Type;

void check_enter_method(char *name, SymbolTable *symbol_table);
void check_exit_method(SymbolTable *symbol_table);
void check_add_prog_param(char *name, Type t, SymbolTable *symbol_table);
void add_type_to_method(Type type, SymbolTable *symbol_table);
void set_method_param_count(int n);
void check_add_fun_param(char *name, Type t, SymbolTable *symbol_table);
void check_add_var(char *name, Type t, SymbolTable *symbol_table);

Type get_type(char *name, SymbolTable *symbol_table);
int get_num_params(char *name, SymbolTable *symbol_table);
Type get_param_type(char *name, SymbolTable *symbol_table);

void fprint_symbol_table(FILE *f, SymbolTable *symbol_table);

#endif