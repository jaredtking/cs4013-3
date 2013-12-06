#include "symbol_table.h"

SymbolTable *new_symbol_table_entry(char *name, Attributes attrs)
{
	SymbolTable *new = (SymbolTable *)malloc(sizeof(SymbolTable));
	new->parent = NULL;
	new->child = NULL;
	new->prev = NULL;
	new->next = NULL;
	new->temp = NULL;

	new->symbol = (Symbol *)malloc(sizeof(Symbol));
	new->symbol->type = attrs.t;
	new->symbol->fun = 0;
	new->symbol->param = 0;
	new->symbol->count = attrs.c;
	new->symbol->array = attrs.a;
	new->symbol->start = attrs.s;
	new->symbol->end = attrs.e;

	new->symbol->name = (char *)malloc(sizeof(name));
	strcpy(new->symbol->name, name);

	return new;
}

Symbol *check_enter_method(char *name, struct ParserData *parser_data)
{
	if (get_symbol(name, parser_data, 1) != NULL)
		semerr("Duplicate method names.", 0, parser_data);

	SymbolTable *new;

	// type: program name if first, otherwise function name
	if (parser_data->sym_eye == NULL)
		new = new_symbol_table_entry(name, (Attributes){PGNAME,0,0,NONE,0,0,0});
	else {
		new = new_symbol_table_entry(name, (Attributes){NONE,0,0,NONE,0,0,0});
		new->symbol->fun = 1;
	}

	// new symbol table
	if (parser_data->symbol_table == NULL) {
		parser_data->symbol_table = new;
	// add entry
	} else if (parser_data->sym_eye != NULL) {
		if (parser_data->sym_turn) {
			parser_data->sym_eye->child = new;
			new->parent = parser_data->sym_eye;
		} else {
			parser_data->sym_eye->next = new;
			new->prev = parser_data->sym_eye;
			new->parent = parser_data->sym_eye->parent;
		}
	}

	parser_data->sym_eye = new;
	parser_data->sym_turn = 1;

	return new->symbol;
}

int check_exit_method(struct ParserData *parser_data)
{
	if (parser_data->sym_eye == NULL) {
		semerr("Cannot exit method before entering.", 0, parser_data);
		return 1;
	}

	// handles the case where we entered a function with no children
	// Problem: upon exit moves eye to the function's parent prematurely
	//          instead of to the function itself
	// Solution: perform a NOP for 1 exit cycle to avoid skipping over function
	if (parser_data->sym_eye->symbol->fun == 1) {
		// now we can move on with our lives..
		if (parser_data->sym_eye->temp != NULL)
			parser_data->sym_eye = parser_data->sym_eye->temp;
		// NOP
		else
			parser_data->sym_eye->temp = parser_data->sym_eye->parent;
	} else
		parser_data->sym_eye = parser_data->sym_eye->parent;

	parser_data->sym_turn = 0;

	return 0;
}

Symbol *check_add_prog_param(char *name, struct ParserData *parser_data)
{
	return check_add_var(name, (Attributes){PGPARAM,0,0,NONE,0,0,0}, parser_data);
}

Symbol *check_add_fun_param(char *name, Attributes attrs, struct ParserData *parser_data)
{
	Symbol *symbol = check_add_var(name, attrs, parser_data);

	symbol->param = 1;

	return symbol;
}

Symbol *check_add_var(char *name, Attributes attrs, struct ParserData *parser_data)
{
	if (parser_data->sym_eye == NULL) {
		semerr("Cannot add variable before entering procedure.", 0, parser_data);
		return NULL;
	}
		
	if (get_symbol(name, parser_data, 0) != NULL)
		semerr("Duplicate variable names.", 0, parser_data);

	SymbolTable *new = new_symbol_table_entry(name, attrs);

	// perform a turn
	if (parser_data->sym_turn) {
		parser_data->sym_eye->child = new;
		new->parent = parser_data->sym_eye;
	// add as sibling
	} else {
		parser_data->sym_eye->next = new;
		new->prev = parser_data->sym_eye;
		new->parent = parser_data->sym_eye->parent;
	}

	parser_data->sym_eye = new;
	parser_data->sym_turn = 0;

	return new->symbol;
}

void set_method_type(Type type, struct ParserData *parser_data)
{
	// handle case where eye is looking at function
	if (parser_data->sym_eye->symbol->fun == 1)
		parser_data->sym_eye->symbol->type = type;
	else if (parser_data->sym_eye->parent != NULL)
		parser_data->sym_eye->parent->symbol->type = type;
}

void set_method_param_count(int count, struct ParserData *parser_data)
{
	// handle case where eye is looking at function
	if (parser_data->sym_eye->symbol->fun == 1)
		parser_data->sym_eye->symbol->count = count;
	else if (parser_data->sym_eye->parent != NULL)
		parser_data->sym_eye->parent->symbol->count = count;
}

SymbolTable *get_symbol(char *name, struct ParserData *parser_data, int global_scope)
{
	if (parser_data->sym_eye == NULL)
		return NULL;

	// STOP conditions:
	// i) !global_scope ^ reached parent of green node
	// ii) global_scope ^ reached NULL
	// iii) always stop on NULL. always.

	SymbolTable *curr = parser_data->sym_eye;

	SymbolTable *stop_condition = NULL;

	if (global_scope == 0) {
		// Avoid jumping up to a new scope when doing a local search
		// on a function after a turn with no children
		if (curr->symbol->fun == 1 && curr->child == NULL)
			stop_condition = curr;
		else
			stop_condition = curr->parent;
	}

	// make sure starting at end of scope
	while (curr->next != NULL)
		curr = curr->next;

	while (curr != NULL && curr != stop_condition) {
		if (strcmp(curr->symbol->name, name) == 0)
			return curr;

		// go to prev until null, then start again at parent
		if (curr->prev != NULL)
			curr = curr->prev;
		else {
			curr = curr->parent;

			if (curr != stop_condition) {
				// make sure starting at end of scope
				while (curr->next != NULL)
					curr = curr->next;
			}
		}
	}

	return NULL;
}

Type get_type(char *name, struct ParserData *parser_data)
{
	SymbolTable *symbol_entry = get_symbol(name, parser_data, 1);

	if (symbol_entry != NULL)
		return symbol_entry->symbol->type;

	return NONE;
}

int get_num_params(char *name, struct ParserData *parser_data)
{
	SymbolTable *symbol_entry = get_symbol(name, parser_data, 1);

	if (symbol_entry != NULL)
		return symbol_entry->symbol->count;

	return 0;
}

Type get_param_type(char *name, int n, struct ParserData *parser_data)
{
	SymbolTable *symbol_entry = get_symbol(name, parser_data, 1);

	if (symbol_entry != NULL) {
		SymbolTable *curr = symbol_entry->child;

		// find the Nth child after function declaration (zero-indexed)
		// the assumption is made that parameters come immediately after method
		int i = 0;
		while (i++ < n && curr != NULL)
			curr = curr->next;

		if (curr != NULL && curr->symbol->param == 1)
			return curr->symbol->type;
	}

	return NONE;
}

char *symbol_attribute_str(Symbol *symbol)
{
	if (symbol->type == PGNAME)
		return "";

	char *type_str;

	switch (symbol->type)
	{
	case PGPARAM: type_str = "param"; break;
	case INT: type_str = "int"; break;
	case REAL: type_str = "real"; break;
	case BOOL: type_str = "bool"; break;
	case ERR: type_str = "err"; break;
	default: type_str = ""; break;
	}

	char *type_str2 = type_str;

	if (symbol->array == 1) {
		type_str2 = (char *)malloc(100);
		sprintf(type_str2, "%s array [%d..%d]", type_str, symbol->start, symbol->end);
	}

	if (symbol->fun == 1) {
		type_str2 = (char *)malloc(100);
		sprintf(type_str2, "%s, fun, %d arg(s)", type_str, symbol->count);
	}

	char *param_str = (symbol->param == 1) ? ", fun param" : "";

	char *str = (char *)malloc(100);
	sprintf(str, "(%s%s)", type_str2, param_str);
	return str;
}

void output_symbol_table(FILE *f, SymbolTable *symbol_table, int level)
{
	SymbolTable *s = symbol_table;

	while (s != NULL && s->symbol != NULL)
	{
		// indent
		for (int l = 0; l < level; l++) fprintf (f, "  ");

		fprintf (f, "%s %s\n", s->symbol->name, symbol_attribute_str(s->symbol));

		// recurse children
		output_symbol_table(f, s->child, level + 1);

		s = s->next;
	}
}

void fprint_symbol_table(FILE *f, SymbolTable *symbol_table)
{
	// symbol table file header
	//fprintf (f, "%-5s%s\n", "Loc.", "ID");

	output_symbol_table(f, symbol_table, 0);
}