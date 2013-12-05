#include "symbol_table.h"

void check_enter_method(char *name, struct ParserData *parser_data)
{

}

void check_exit_method(struct ParserData *parser_data)
{

}

void set_method_type(Type type, struct ParserData *parser_data)
{

}

void set_method_param_count(int n, struct ParserData *parser_data)
{

}

void check_add_prog_param(char *name, Type type, struct ParserData *parser_data)
{

}

void check_add_fun_param(char *name, Type type, struct ParserData *parser_data)
{

}

void check_add_var(char *name, Type type, struct ParserData *parser_data)
{

}

Symbol *get_symbol(char *name, SymbolTable *symbol_table, int global_scope)
{
	// empty table
	if (symbol_table->symbol == NULL)
		return NULL;
	// symbol found
	else if (strcmp(symbol_table->symbol->name, name) == 0)
		return symbol_table->symbol;
	// end of non-empty table
	else if (symbol_table->next == NULL)
		return NULL;
	// try next entry
	else
		return get_symbol(name, symbol_table->next, global_scope);
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
		fprintf (f, "%-5d%s\n", i, s->symbol->name);

		i++;
		s = s->next;
	}
}

// NOTE get_type should return return type if type = FNAME