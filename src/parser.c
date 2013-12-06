#include "parser.h"

void parse(ParserData *parser_data)
{
	parse_program(parser_data);

	match(TOKEN_EOF, parser_data);

	// output symbol table
	fprint_symbol_table(parser_data->symbols, parser_data->symbol_table);
}

struct MachineResult *match(TokenType t, ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NONE);

	if (tok->token->type == t) {
		if (t == TOKEN_EOF) {
			// END
		} else {
			// NOP
		}

		return tok;
	} else {
		synerr((TokenType[]){t}, 1, tok, parser_data);
		return NULL;
	}
}

void parse_program(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	MachineResult *idptr;
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_PROGRAM:
		if (match(TOKEN_PROGRAM, parser_data) == NULL) { doSynch = 1; break; }

		idptr = match(TOKEN_ID, parser_data);
		if (idptr == NULL) { doSynch = 1; break; }

		check_enter_method(idptr->lexeme, parser_data);

		if (match(TOKEN_LPAREN, parser_data) == NULL) { doSynch = 1; break; }
		parse_id_list(parser_data);
		if (match(TOKEN_RPAREN, parser_data) == NULL) { doSynch = 1; break; }
		if (match(TOKEN_SEMICOLON, parser_data) == NULL) { doSynch = 1; break; }
		parse_program_(parser_data);

		check_exit_method(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_PROGRAM}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_PROGRAM, tok, parser_data);
}

void parse_program_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_VAR:
		parse_declarations(parser_data);
		parse_program__(parser_data);
	break;
	case TOKEN_FUNCTION:
		parse_subprogram_declarations(parser_data);
		parse_compound_statement(parser_data);
		if (match(TOKEN_PERIOD, parser_data) == NULL) { doSynch = 1; break; }
	break;
	case TOKEN_BEGIN:
		parse_compound_statement(parser_data);
		if (match(TOKEN_PERIOD, parser_data) == NULL) { doSynch = 1; break; }
	break;
	default:
		synerr((TokenType[]){TOKEN_VAR,TOKEN_FUNCTION,TOKEN_BEGIN}, 3, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_PROGRAM_, tok, parser_data);
}

void parse_program__(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_FUNCTION:
		parse_subprogram_declarations(parser_data);
		parse_compound_statement(parser_data);
		if (match(TOKEN_PERIOD, parser_data) == NULL) { doSynch = 1; break; }
	break;
	case TOKEN_BEGIN:
		parse_compound_statement(parser_data);
		if (match(TOKEN_PERIOD, parser_data) == NULL) { doSynch = 1; break; }
	break;
	default:
		synerr((TokenType[]){TOKEN_FUNCTION,TOKEN_BEGIN}, 2, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_PROGRAM__, tok, parser_data);
}

void parse_id_list(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	MachineResult *idptr;
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_ID:
		idptr = match(TOKEN_ID, parser_data);
		if (idptr == NULL) { doSynch = 1; break; }
		check_add_prog_param(idptr->lexeme, parser_data);

		parse_id_list_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_ID_LIST, tok, parser_data);
}

void parse_id_list_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	MachineResult *idptr;
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_COMMA:
		if (match(TOKEN_COMMA, parser_data) == NULL) { doSynch = 1; break; }

		idptr = match(TOKEN_ID, parser_data);
		if (idptr == NULL) { doSynch = 1; break; }
		check_add_prog_param(idptr->lexeme, parser_data);

		parse_id_list_(parser_data);
	break;
	case TOKEN_RPAREN:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_COMMA,TOKEN_RPAREN}, 2, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_ID_LIST_, tok, parser_data);
}

void parse_declarations(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	MachineResult *idptr;
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_VAR:
		if (match(TOKEN_VAR, parser_data) == NULL) { doSynch = 1; break; }
		idptr = match(TOKEN_ID, parser_data);
		if (idptr == NULL) { doSynch = 1; break; }
		if (match(TOKEN_COLON, parser_data) == NULL) { doSynch = 1; break; }
		Attributes t = parse_type(parser_data);
		check_add_var(idptr->lexeme, t, parser_data);
		if (match(TOKEN_SEMICOLON, parser_data) == NULL) { doSynch = 1; break; }
		parse_declarations_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_VAR}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_DECLARATIONS, tok, parser_data);
}

void parse_declarations_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	MachineResult *idptr;
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_VAR:
		if (match(TOKEN_VAR, parser_data) == NULL) { doSynch = 1; break; }
		idptr = match(TOKEN_ID, parser_data);
		if (idptr == NULL) { doSynch = 1; break; }
		if (match(TOKEN_COLON, parser_data) == NULL) { doSynch = 1; break; }
		Attributes t = parse_type(parser_data);
		check_add_var(idptr->lexeme, t, parser_data);
		if (match(TOKEN_SEMICOLON, parser_data) == NULL) { doSynch = 1; break; }
		parse_declarations_(parser_data);
	break;
	case TOKEN_FUNCTION:
	case TOKEN_BEGIN:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_VAR, TOKEN_FUNCTION, TOKEN_BEGIN}, 3, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_DECLARATIONS_, tok, parser_data);
}

struct Attributes parse_type(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	Attributes t = ATTRIBUTES_DEFAULT, st = ATTRIBUTES_DEFAULT;
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_ARRAY:
		if (match(TOKEN_ARRAY, parser_data) == NULL) { doSynch = 1; break; }
		if (match(TOKEN_LBRACKET, parser_data) == NULL) { doSynch = 1; break; }
		MachineResult *num1 = match(TOKEN_NUM, parser_data);
		if (num1 == NULL) { doSynch = 1; break; }
		if (match(TOKEN_PERIOD, parser_data) == NULL) { doSynch = 1; break; }
		if (match(TOKEN_PERIOD, parser_data) == NULL) { doSynch = 1; break; }
		if (match(TOKEN_RBRACKET, parser_data) == NULL) { doSynch = 1; break; }
		MachineResult *num2 = match(TOKEN_NUM, parser_data);
		if (num2 == NULL) { doSynch = 1; break; }
		if (match(TOKEN_OF, parser_data) == NULL) { doSynch = 1; break; }
		st = parse_std_type(parser_data);

		if (num1->token->attribute != ATTRIBUTE_INT || num2->token->attribute != ATTRIBUTE_INT) {
			// TODO ERR*
			t.t = ERR;
		} else {
			int num1val;
			sscanf(num1->lexeme, "%d", &num1val);
			int num2val;
			sscanf(num2->lexeme, "%d", &num2val);

			st.a = 1;
			st.s = num1val;
			st.e = num2val;

			if (num2val > num1val) {
				// TODO ERR*
				t.t = ERR;
			}
		}
	break;
	case TOKEN_INTEGER:
	case TOKEN_REAL:
		st = parse_std_type(parser_data);
		t.t = st.t;
	break;
	default:
		synerr((TokenType[]){TOKEN_INTEGER, TOKEN_REAL, TOKEN_ARRAY}, 3, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_TYPE, tok, parser_data);

	return t;
}

struct Attributes parse_std_type(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	Attributes st = ATTRIBUTES_DEFAULT;
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_INTEGER:
		if (match(TOKEN_INTEGER, parser_data) == NULL) { doSynch = 1; break; }
		st.t = INT;
	break;
	case TOKEN_REAL:
		if (match(TOKEN_REAL, parser_data) == NULL) { doSynch = 1; break; }
		st.t = REAL;
	break;
	default:
		synerr((TokenType[]){TOKEN_INTEGER, TOKEN_REAL}, 2, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_STD_TYPE, tok, parser_data);

	return st;
}

void parse_subprogram_declarations(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_FUNCTION:
		parse_subprogram_declaration(parser_data);
		if (match(TOKEN_SEMICOLON, parser_data) == NULL) { doSynch = 1; break; }
		parse_subprogram_declarations_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_FUNCTION}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_SUBPROGRAM_DECLARATIONS, tok, parser_data);
}

void parse_subprogram_declarations_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_FUNCTION:
		parse_subprogram_declaration(parser_data);
		if (match(TOKEN_SEMICOLON, parser_data) == NULL) { doSynch = 1; break; }
		parse_subprogram_declarations_(parser_data);
	break;
	case TOKEN_BEGIN:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_FUNCTION,TOKEN_BEGIN}, 2, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_SUBPROGRAM_DECLARATIONS_, tok, parser_data);
}

void parse_subprogram_declaration(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_FUNCTION:
		parse_subprogram_head(parser_data);
		parse_subprogram_declaration_(parser_data);
		check_exit_method(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_FUNCTION}, 1, tok, parser_data);
		synch(PRODUCTION_SUBPROGRAM_DECLARATION, tok, parser_data);
	break;
	}
}

void parse_subprogram_declaration_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_VAR:
		parse_declarations(parser_data);
		parse_subprogram_declaration__(parser_data);
	break;
	case TOKEN_FUNCTION:
		parse_subprogram_declarations(parser_data);
		parse_compound_statement(parser_data);
	break;
	case TOKEN_BEGIN:
		parse_compound_statement(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_VAR,TOKEN_FUNCTION,TOKEN_BEGIN}, 3, tok, parser_data);
		synch(PRODUCTION_SUBPROGRAM_DECLARATION_, tok, parser_data);
	break;
	}
}

void parse_subprogram_declaration__(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_FUNCTION:
		parse_subprogram_declarations(parser_data);
		parse_compound_statement(parser_data);
	break;
	case TOKEN_BEGIN:
		parse_compound_statement(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_FUNCTION,TOKEN_BEGIN}, 2, tok, parser_data);
		synch(PRODUCTION_SUBPROGRAM_DECLARATION__, tok, parser_data);
	break;
	}
}

void parse_subprogram_head(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	MachineResult *idptr;
	int doSynch = 0;
	Attributes sh_ = ATTRIBUTES_DEFAULT;

	switch (tok->token->type) {
	case TOKEN_FUNCTION:
		if (match(TOKEN_FUNCTION, parser_data) == NULL) { doSynch = 1; break; }
		idptr = match(TOKEN_ID, parser_data);
		if (idptr == NULL) { doSynch = 1; break; }
		check_enter_method(idptr->lexeme, parser_data);
		sh_ = parse_subprogram_head_(parser_data);
		set_method_type(sh_.t, parser_data);
    	set_method_param_count(sh_.c, parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_FUNCTION}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_SUBPROGRAM_HEAD, tok, parser_data);
}

struct Attributes parse_subprogram_head_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;
	Attributes sh_ = ATTRIBUTES_DEFAULT, a = ATTRIBUTES_DEFAULT, st = ATTRIBUTES_DEFAULT;

	switch (tok->token->type) {
	case TOKEN_LPAREN:
		a = parse_arguments(parser_data);
		sh_.c = a.c;
		if (match(TOKEN_COLON, parser_data) == NULL) { doSynch = 1; break; }
		st = parse_std_type(parser_data);
		sh_.t = st.t;
		if (match(TOKEN_SEMICOLON, parser_data) == NULL) { doSynch = 1; break; }
	break;
	case TOKEN_COLON:
		sh_.c = 0;
		if (match(TOKEN_COLON, parser_data) == NULL) { doSynch = 1; break; }
		st = parse_std_type(parser_data);
		sh_.t = st.t;
		if (match(TOKEN_SEMICOLON, parser_data) == NULL) { doSynch = 1; break; }
	break;
	default:
		synerr((TokenType[]){TOKEN_LPAREN,TOKEN_COLON}, 2, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_SUBPROGRAM_HEAD_, tok, parser_data);

	return sh_;
}

struct Attributes parse_arguments(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	Attributes a = ATTRIBUTES_DEFAULT, pl = ATTRIBUTES_DEFAULT;
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_LPAREN:
		if (match(TOKEN_LPAREN, parser_data) == NULL) { doSynch = 1; break; }
		pl = parse_param_list(parser_data);
		a.c = pl.c;
		if (match(TOKEN_RPAREN, parser_data) == NULL) { doSynch = 1; break; }
	break;
	default:
		synerr((TokenType[]){TOKEN_LPAREN}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_ARGUMENTS, tok, parser_data);

	return a;
}

struct Attributes parse_param_list(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	MachineResult *idptr;
	int doSynch = 0;
	Attributes pl = ATTRIBUTES_DEFAULT, t = ATTRIBUTES_DEFAULT, pl_ = ATTRIBUTES_DEFAULT;

	switch (tok->token->type) {
	case TOKEN_ID:
		idptr = match(TOKEN_ID, parser_data);
		if (idptr == NULL) { doSynch = 1; break; }
		if (match(TOKEN_COLON, parser_data) == NULL) { doSynch = 1; break; }
		t = parse_type(parser_data);
		check_add_fun_param(idptr->lexeme, t, parser_data);
		pl_.in = 1;
		parse_param_list_(parser_data, &pl_);
		pl.c = pl_.c;
	break;
	default:
		synerr((TokenType[]){TOKEN_ID}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_PARAM_LIST, tok, parser_data);

	return pl;
}

void parse_param_list_(ParserData *parser_data, struct Attributes *pl_)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	MachineResult *idptr;
	int doSynch = 0;
	Attributes t = ATTRIBUTES_DEFAULT, pl1_ = ATTRIBUTES_DEFAULT;

	switch (tok->token->type) {
	case TOKEN_SEMICOLON:
		if (match(TOKEN_SEMICOLON, parser_data) == NULL) { doSynch = 1; break; }
		idptr = match(TOKEN_ID, parser_data);
		if (idptr == NULL) { doSynch = 1; break; }
		if (match(TOKEN_COLON, parser_data) == NULL) { doSynch = 1; break; }
		t = parse_type(parser_data);
		check_add_fun_param(idptr->lexeme, t, parser_data);
		pl1_.in = pl_->in + 1;
		parse_param_list_(parser_data, &pl1_);
		pl_->c = pl1_.c;
	break;
	case TOKEN_RPAREN:
		// NOP
		pl_->c = pl_->in;
	break;
	default:
		synerr((TokenType[]){TOKEN_COLON,TOKEN_RPAREN}, 2, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_PARAM_LIST_, tok, parser_data);
}

void parse_compound_statement(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_BEGIN:
		if (match(TOKEN_BEGIN, parser_data) == NULL) { doSynch = 1; break; }
		parse_compound_statement_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_BEGIN}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_COMPOUND_STATEMENT, tok, parser_data);
}

void parse_compound_statement_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_ID:
	case TOKEN_BEGIN:
	case TOKEN_IF:
	case TOKEN_WHILE:
		parse_optional_statements(parser_data);
	case TOKEN_END:
		if (match(TOKEN_END, parser_data) == NULL) { doSynch = 1; break; }
	break;
	default:
		synerr((TokenType[]){TOKEN_ID,TOKEN_BEGIN,TOKEN_IF,TOKEN_WHILE,TOKEN_END}, 5, tok, parser_data);
		doSynch = 1;
	break;
	}	

	if (doSynch == 1)
		synch(PRODUCTION_COMPOUND_STATEMENT_, tok, parser_data);	
}

void parse_optional_statements(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_ID:
	case TOKEN_BEGIN:
	case TOKEN_IF:
	case TOKEN_WHILE:
		parse_statement_list(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID,TOKEN_BEGIN,TOKEN_IF,TOKEN_WHILE}, 4, tok, parser_data);
		synch(PRODUCTION_OPTIONAL_STATEMENTS, tok, parser_data);
	break;
	}
}

void parse_statement_list(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_ID:
	case TOKEN_BEGIN:
	case TOKEN_IF:
	case TOKEN_WHILE:
		parse_statement(parser_data);
		parse_statement_list_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID,TOKEN_BEGIN,TOKEN_IF,TOKEN_WHILE}, 4, tok, parser_data);
		synch(PRODUCTION_STATEMENT_LIST, tok, parser_data);
	break;
	}
}

void parse_statement_list_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_SEMICOLON:
		if (match(TOKEN_SEMICOLON, parser_data) == NULL) { doSynch = 1; break; }
		parse_statement(parser_data);
		parse_statement_list_(parser_data);
	break;
	case TOKEN_END:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_SEMICOLON,TOKEN_END}, 2, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_STATEMENT_LIST_, tok, parser_data);
}

void parse_statement(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_ID:
		parse_var(parser_data);
		if (match(TOKEN_ASSIGNOP, parser_data) == NULL) { doSynch = 1; break; }
		parse_expr(parser_data);
	break;
	case TOKEN_BEGIN:
		parse_compound_statement(parser_data);
	break;
	case TOKEN_IF:
		if (match(TOKEN_IF, parser_data) == NULL) { doSynch = 1; break; }
		parse_expr(parser_data);
		if (match(TOKEN_THEN, parser_data) == NULL) { doSynch = 1; break; }
		parse_statement(parser_data);
		parse_statement_(parser_data);
	break;
	case TOKEN_WHILE:
		if (match(TOKEN_WHILE, parser_data) == NULL) { doSynch = 1; break; }
		parse_expr(parser_data);
		if (match(TOKEN_DO, parser_data) == NULL) { doSynch = 1; break; }
		parse_statement(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID,TOKEN_BEGIN,TOKEN_IF,TOKEN_WHILE}, 4, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_STATEMENT, tok, parser_data);
}

void parse_statement_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_ELSE:
		tok = get_next_token(parser_data, TOKEN_OPTION_NONE);
		if (tok->token->type == TOKEN_ELSE) {
			parse_statement(parser_data);
		} else  {
			// NOP
		}
	break;
	case TOKEN_SEMICOLON:
	case TOKEN_END:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_ELSE,TOKEN_SEMICOLON,TOKEN_END}, 4, tok, parser_data);
		synch(PRODUCTION_STATEMENT_, tok, parser_data);
	break;
	}
}

void parse_var(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_ID:
		if (match(TOKEN_ID, parser_data) == NULL) { doSynch = 1; break; }
		parse_var_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_VAR, tok, parser_data);
}

void parse_var_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_LBRACKET:
		if (match(TOKEN_LBRACKET, parser_data) == NULL) { doSynch = 1; break; }
		parse_expr(parser_data);
		if (match(TOKEN_RBRACKET, parser_data) == NULL) { doSynch = 1; break; }
	break;
	case TOKEN_ASSIGNOP:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_LBRACKET,TOKEN_ASSIGNOP}, 2, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_VAR_, tok, parser_data);
}

void parse_expr_list(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_ID:
	case TOKEN_NUM:
	case TOKEN_LPAREN:
	case TOKEN_NOT:
	case TOKEN_ADDOP:
		parse_expr(parser_data);
		parse_expr_list_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID,TOKEN_NUM,TOKEN_LPAREN,TOKEN_NOT,TOKEN_ADDOP}, 5, tok, parser_data);
		synch(PRODUCTION_EXPR_LIST, tok, parser_data);
	break;
	}
}

void parse_expr_list_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_COMMA:
		if (match(TOKEN_COMMA, parser_data) == NULL) { doSynch = 1; break; }
		parse_expr(parser_data);
		parse_expr_list_(parser_data);
	break;
	case TOKEN_RPAREN:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_COMMA,TOKEN_RPAREN}, 2, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_EXPR_LIST_, tok, parser_data);
}

void parse_expr(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_ID:
	case TOKEN_NUM:
	case TOKEN_LPAREN:
	case TOKEN_NOT:
	case TOKEN_ADDOP:
		parse_simple_expr(parser_data);
		parse_expr_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID,TOKEN_NUM,TOKEN_LPAREN,TOKEN_NOT,TOKEN_ADDOP}, 5, tok, parser_data);
		synch(PRODUCTION_EXPR, tok, parser_data);
	break;
	}
}

void parse_expr_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_RELOP:
		if (match(TOKEN_RELOP, parser_data) == NULL) { doSynch = 1; break; }
		parse_simple_expr(parser_data);
	break;
	case TOKEN_SEMICOLON:
	case TOKEN_END:
	case TOKEN_ELSE:
	case TOKEN_THEN:
	case TOKEN_DO:
	case TOKEN_RBRACKET:
	case TOKEN_COMMA:
	case TOKEN_RPAREN:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_RELOP,TOKEN_SEMICOLON,TOKEN_END,TOKEN_ELSE,TOKEN_THEN,TOKEN_DO,TOKEN_RBRACKET,TOKEN_COMMA,TOKEN_RPAREN}, 9, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_EXPR_, tok, parser_data);
}

void parse_simple_expr(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_ID:
	case TOKEN_NUM:
	case TOKEN_LPAREN:
	case TOKEN_NOT:
	case TOKEN_ADDOP:
		parse_term(parser_data);
		parse_simple_expr_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID,TOKEN_NUM,TOKEN_LPAREN,TOKEN_NOT,TOKEN_ADDOP}, 5, tok, parser_data);
		synch(PRODUCTION_SIMPLE_EXPR, tok, parser_data);
	break;
	}
}

void parse_simple_expr_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_ADDOP:
		if (match(TOKEN_ADDOP, parser_data) == NULL) { doSynch = 1; break; }
		parse_term(parser_data);
		parse_simple_expr_(parser_data);
	break;
	case TOKEN_RELOP:
	case TOKEN_SEMICOLON:
	case TOKEN_END:
	case TOKEN_ELSE:
	case TOKEN_THEN:
	case TOKEN_DO:
	case TOKEN_RBRACKET:
	case TOKEN_COMMA:
	case TOKEN_RPAREN:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_RELOP,TOKEN_SEMICOLON,TOKEN_END,TOKEN_ELSE,TOKEN_THEN,TOKEN_DO,TOKEN_RBRACKET,TOKEN_COMMA,TOKEN_RPAREN}, 10, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_SIMPLE_EXPR_, tok, parser_data);
}

void parse_term(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

	switch (tok->token->type) {
	case TOKEN_ID:
	case TOKEN_NUM:
	case TOKEN_LPAREN:
	case TOKEN_NOT:
		parse_factor(parser_data);
		parse_term_(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID,TOKEN_NUM,TOKEN_LPAREN,TOKEN_NOT}, 4, tok, parser_data);
		synch(PRODUCTION_TERM, tok, parser_data);
	break;
	}
}

void parse_term_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_MULOP:
		if (match(TOKEN_MULOP, parser_data) == NULL) { doSynch = 1; break; }
		parse_factor(parser_data);
		parse_term_(parser_data);
	break;
	case TOKEN_ADDOP:
	case TOKEN_RELOP:
	case TOKEN_SEMICOLON:
	case TOKEN_END:
	case TOKEN_ELSE:
	case TOKEN_THEN:
	case TOKEN_DO:
	case TOKEN_RBRACKET:
	case TOKEN_COMMA:
	case TOKEN_RPAREN:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_MULOP,TOKEN_ADDOP,TOKEN_RELOP,TOKEN_SEMICOLON,TOKEN_END,TOKEN_ELSE,TOKEN_THEN,TOKEN_DO,TOKEN_RBRACKET,TOKEN_COMMA,TOKEN_RPAREN}, 11, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_TERM_, tok, parser_data);
}

void parse_factor(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_ID:
		if (match(TOKEN_ID, parser_data) == NULL) { doSynch = 1; break; }
		parse_factor_(parser_data);
	break;
	case TOKEN_NUM:
		if (match(TOKEN_NUM, parser_data) == NULL) { doSynch = 1; break; }
	break;
	case TOKEN_LPAREN:
		if (match(TOKEN_LPAREN, parser_data) == NULL) { doSynch = 1; break; }
		parse_expr(parser_data);
		if (match(TOKEN_RPAREN, parser_data) == NULL) { doSynch = 1; break; }
	break;
	case TOKEN_NOT:
		if (match(TOKEN_NOT, parser_data) == NULL) { doSynch = 1; break; }
		parse_factor(parser_data);
	break;
	default:
		synerr((TokenType[]){TOKEN_ID,TOKEN_NUM,TOKEN_LPAREN,TOKEN_NOT}, 4, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_FACTOR, tok, parser_data);
}

void parse_factor_(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_LPAREN:
		if (match(TOKEN_LPAREN, parser_data) == NULL) { doSynch = 1; break; }
		parse_expr_list(parser_data);
		if (match(TOKEN_RPAREN, parser_data) == NULL) { doSynch = 1; break; }
	break;
	case TOKEN_LBRACKET:
		if (match(TOKEN_LBRACKET, parser_data) == NULL) { doSynch = 1; break; }
		parse_expr(parser_data);
		if (match(TOKEN_RBRACKET, parser_data) == NULL) { doSynch = 1; break; }
	break;
	case TOKEN_MULOP:
	case TOKEN_ADDOP:
	case TOKEN_RELOP:
	case TOKEN_SEMICOLON:
	case TOKEN_END:
	case TOKEN_ELSE:
	case TOKEN_THEN:
	case TOKEN_DO:
	case TOKEN_RBRACKET:
	case TOKEN_COMMA:
	case TOKEN_RPAREN:
		// NOP
	break;
	default:
		synerr((TokenType[]){TOKEN_LPAREN,TOKEN_LBRACKET,TOKEN_MULOP,TOKEN_ADDOP,TOKEN_RELOP,TOKEN_SEMICOLON,TOKEN_END,TOKEN_ELSE,TOKEN_THEN,TOKEN_DO,TOKEN_RBRACKET,TOKEN_COMMA,TOKEN_RPAREN}, 13, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_FACTOR_, tok, parser_data);
}

void parse_sign(ParserData *parser_data)
{
	MachineResult *tok = get_next_token(parser_data, TOKEN_OPTION_NOP);
	int doSynch = 0;

	switch (tok->token->type) {
	case TOKEN_ADDOP:
		if (match(TOKEN_ADDOP, parser_data) == NULL) { doSynch = 1; break; }
	break;
	default:
		synerr((TokenType[]){TOKEN_ADDOP}, 1, tok, parser_data);
		doSynch = 1;
	break;
	}

	if (doSynch == 1)
		synch(PRODUCTION_SIGN, tok, parser_data);
}

void synch(Production prod, MachineResult *tok, ParserData *parser_data)
{
	TokenType *synchSet;
	int len;

	switch(prod) {
	case PRODUCTION_PROGRAM:
	case PRODUCTION_PROGRAM_:
	case PRODUCTION_PROGRAM__:
		synchSet = (TokenType[]){TOKEN_EOF}; len = 1;
	break;
	case PRODUCTION_ID_LIST:
	case PRODUCTION_ID_LIST_:
	case PRODUCTION_PARAM_LIST:
	case PRODUCTION_PARAM_LIST_:
	case PRODUCTION_EXPR_LIST:
	case PRODUCTION_EXPR_LIST_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_RPAREN}; len = 2;
	break;
	case PRODUCTION_DECLARATIONS:
	case PRODUCTION_DECLARATIONS_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_BEGIN, TOKEN_SEMICOLON, TOKEN_PROCEDURE}; len = 4;
	break;
	case PRODUCTION_TYPE:
	case PRODUCTION_STD_TYPE:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_SEMICOLON, TOKEN_RPAREN}; len = 3;
	break;
	case PRODUCTION_SUBPROGRAM_DECLARATIONS:
	case PRODUCTION_SUBPROGRAM_DECLARATIONS_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_BEGIN, TOKEN_SEMICOLON}; len = 3;
	break;
	case PRODUCTION_SUBPROGRAM_DECLARATION:
	case PRODUCTION_SUBPROGRAM_DECLARATION_:
	case PRODUCTION_SUBPROGRAM_DECLARATION__:
	case PRODUCTION_ARGUMENTS:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_SEMICOLON}; len = 2;
	break;
	case PRODUCTION_COMPOUND_STATEMENT:
	case PRODUCTION_COMPOUND_STATEMENT_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_PERIOD, TOKEN_END, TOKEN_ELSE, TOKEN_SEMICOLON}; len = 5;
	break;

	case PRODUCTION_SUBPROGRAM_HEAD:
	case PRODUCTION_SUBPROGRAM_HEAD_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_BEGIN, TOKEN_VAR, TOKEN_SEMICOLON, TOKEN_PROCEDURE}; len = 5;
	break;
	case PRODUCTION_OPTIONAL_STATEMENTS:
	case PRODUCTION_STATEMENT_LIST:
	case PRODUCTION_STATEMENT_LIST_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_END}; len = 2;
	break;
	case PRODUCTION_STATEMENT:
	case PRODUCTION_STATEMENT_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_END, TOKEN_ELSE, TOKEN_SEMICOLON}; len = 4;
	break;
	case PRODUCTION_VAR:
	case PRODUCTION_VAR_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_ASSIGNOP}; len = 2;
	break;
	case PRODUCTION_EXPR:
	case PRODUCTION_EXPR_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_THEN, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_RBRACKET, TOKEN_DO, TOKEN_ELSE, TOKEN_RPAREN, TOKEN_END}; len = 9;
	break;
	case PRODUCTION_SIMPLE_EXPR:
	case PRODUCTION_SIMPLE_EXPR_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_THEN, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_RBRACKET, TOKEN_DO, TOKEN_ELSE, TOKEN_RPAREN, TOKEN_END, TOKEN_RELOP}; len = 10;
	break;
	case PRODUCTION_TERM:
	case PRODUCTION_TERM_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_THEN, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_RBRACKET, TOKEN_DO, TOKEN_ELSE, TOKEN_RPAREN, TOKEN_END, TOKEN_RELOP, TOKEN_ADDOP}; len = 11;
	break;
	case PRODUCTION_FACTOR:
	case PRODUCTION_FACTOR_:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_THEN, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_RBRACKET, TOKEN_DO, TOKEN_ELSE, TOKEN_RPAREN, TOKEN_END, TOKEN_RELOP, TOKEN_ADDOP, TOKEN_MULOP}; len = 12;
	break;
	case PRODUCTION_SIGN:
		synchSet = (TokenType[]){TOKEN_EOF, TOKEN_ID, TOKEN_NUM, TOKEN_LPAREN, TOKEN_NOT}; len = 5;
	break;
	}

	while(1) {
		// advance to next token but hang onto it
		tok = get_next_token(parser_data, TOKEN_OPTION_NOP);

		for(int i = 0; i < len; i++) {
			if(tok->token->type == synchSet[i]) {
				if(tok->token->type == TOKEN_EOF) {
					fprintf(stderr, "Reached EOF...quitting now\n");
					exit(PARSER_RESULT_SYNERR);
				} else {
					fprintf(stderr, "Synchronized on term \"%s\" (%s)\n", tok->lexeme, token_type_to_str(tok->token->type));
					return;
				}
			}
		}

		// advance to next token
		tok = get_next_token(parser_data, TOKEN_OPTION_NONE);
	}
}