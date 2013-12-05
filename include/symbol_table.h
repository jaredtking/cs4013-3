#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "errors.h"

typedef enum Type
{
	PGNAME,
	FNAME,
	INT,
	REAL
} Type;

typedef struct Symbol
{
	char *name;
	Type *type;
} Symbol;

typedef struct SymbolTable
{
	Symbol *symbol;
	struct SymbolTable *next;
	struct SymbolTable *prev;
	struct SymbolTable *parent;
	struct SymbolTable *child;
} SymbolTable;

struct ParserData;

void check_enter_method(char *name, struct ParserData *parser_data);
void check_exit_method(struct ParserData *parser_data);
void set_method_type(Type type, struct ParserData *parser_data);
void set_method_param_count(int n, struct ParserData *parser_data);
void check_add_prog_param(char *name, Type type, struct ParserData *parser_data);
void check_add_fun_param(char *name, Type type, struct ParserData *parser_data);
void check_add_var(char *name, Type type, struct ParserData *parser_data);

Type get_type(char *name, SymbolTable *symbol_table);
int get_num_params(char *name, SymbolTable *symbol_table);
Type get_param_type(char *name, SymbolTable *symbol_table);

Symbol *get_symbol(char *name, SymbolTable *symbol_table, int global_scope);

void fprint_symbol_table(FILE *f, SymbolTable *symbol_table);

#endif