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

#endif