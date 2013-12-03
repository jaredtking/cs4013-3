#include "symbol_table.h"

int get_sym_table_addr(char *word, SymbolTable *symbol_table, int loc)
{
	// empty table
	if (symbol_table->symbol == NULL)
	{
		symbol_table->symbol = (char *)malloc(strlen(word)+1*sizeof(char));
		strcpy(symbol_table->symbol, word);
		return loc;
	}
	// symbol found
	else if (strcmp(symbol_table->symbol, word) == 0)
		return loc;
	// end of non-empty table
	else if (symbol_table->next == NULL)
	{
		SymbolTable *symbol = (SymbolTable *)malloc(sizeof(SymbolTable));
		symbol->symbol = (char *)malloc(strlen(word)+1*sizeof(char));
		strcpy(symbol->symbol, word);
		symbol->next = NULL;

		symbol_table->next = symbol;

		return loc + 1;
	}
	// try next entry
	else
		return get_sym_table_addr(word, symbol_table->next, loc + 1);
}

void fprint_symbol_table(FILE *f, SymbolTable *symbol_table)
{
	// symbol table file header
	fprintf (f, "%-5s%s\n", "Loc.", "ID");

	// write ids to symbol table
	SymbolTable *s = symbol_table;
	int i = 0;
	while (s != NULL && s->symbol != NULL)
	{
		fprintf (f, "%-5d%s\n", i, s->symbol);

		i++;
		s = s->next;
	}
}