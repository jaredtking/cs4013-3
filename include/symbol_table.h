#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "errors.h"

typedef enum Type
{
	NONE,
	PGNAME,
	PGPARAM,
	INT,
	REAL,
	BOOL,
	ERR
} Type;

typedef struct Symbol
{
	char *name;
	Type type;
	int fun;
	int param;
	int count;
	int array;
	int start;
	int end;
} Symbol;

typedef struct SymbolTable
{
	Symbol *symbol;
	struct SymbolTable *next;
	struct SymbolTable *prev;
	struct SymbolTable *parent;
	struct SymbolTable *child;
	struct SymbolTable *temp;
} SymbolTable;

typedef struct Attributes
{
	Type t;
	int c;
	int in;
	Type in_t;
	int a;
	int s;
	int e;
} Attributes;

#define ATTRIBUTES_DEFAULT (Attributes){NONE,0,0,NONE,0,0,0}

struct ParserData;

Symbol *check_enter_method(char *name, struct ParserData *parser_data);
int check_exit_method(struct ParserData *parser_data);
Symbol *check_add_prog_param(char *name, struct ParserData *parser_data);
Symbol *check_add_fun_param(char *name, Attributes attrs, struct ParserData *parser_data);
Symbol *check_add_var(char *name, Attributes attrs, struct ParserData *parser_data);
void set_method_type(Type type, struct ParserData *parser_data);
void set_method_param_count(int count, struct ParserData *parser_data);

SymbolTable *get_symbol(char *name, struct ParserData *parser_data, int global_scope);
Type get_type(char *name, struct ParserData *parser_data);
int get_num_params(char *name, struct ParserData *parser_data);
Type get_param_type(char *name, int n, struct ParserData *parser_data);

void fprint_symbol_table(FILE *f, SymbolTable *symbol_table);

#endif