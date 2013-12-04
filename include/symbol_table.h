#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum Type
{
	PGNAME,
	FNAME,
	INT,
	REAL
} Type;

typedef enum Symbol
{
	char *name;
	Type *type;
}

typedef struct SymbolTable
{
	Symbol *symbol;
	struct SymbolTable *next;
	struct SymbolTable *prev;
	struct SymbolTable *parent;
	struct SymbolTable *child;
} SymbolTable;

void check_enter_method(char *name, ParserData *parser_data);
void check_exit_method(ParserData *parser_data);
void set_method_type(Type type, ParserData *parser_data);
void set_method_param_count(int n, ParserData *parser_data);
void check_add_prog_param(char *name, Type type, ParserData *parser_data);
void check_add_fun_param(char *name, Type type, ParserData *parser_data);
void check_add_var(char *name, Type type, ParserData *parser_data);

Type get_type(char *name, SymbolTable *symbol_table);
int get_num_params(char *name, SymbolTable *symbol_table);
Type get_param_type(char *name, SymbolTable *symbol_table);

void fprint_symbol_table(FILE *f, SymbolTable *symbol_table);

#endif