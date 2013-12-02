#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "token.h"

void parse(ParserData *parser_data);
void match(TokenType t, ParserData *parser_data);

void parse_program(ParserData *parser_data);
void parse_program_(ParserData *parser_data);
void parse_program__(ParserData *parser_data);
void parse_id_list(ParserData *parser_data);
void parse_id_list_(ParserData *parser_data);
void parse_declarations(ParserData *parser_data);
void parse_declarations_(ParserData *parser_data);
void parse_type(ParserData *parser_data);
void parse_std_type(ParserData *parser_data);
void parse_subprogram_declarations(ParserData *parser_data);
void parse_subprogram_declarations_(ParserData *parser_data);
void parse_subprogram_declaration(ParserData *parser_data);
void parse_subprogram_declaration_(ParserData *parser_data);
void parse_subprogram_declaration__(ParserData *parser_data);
void parse_subprogram_head(ParserData *parser_data);
void parse_subprogram_head_(ParserData *parser_data);
void parse_arguments(ParserData *parser_data);
void parse_param_list(ParserData *parser_data);
void parse_param_list_(ParserData *parser_data);
void parse_compound_statement(ParserData *parser_data);
void parse_compound_statement_(ParserData *parser_data);
void parse_optional_statements(ParserData *parser_data);
void parse_statement_list(ParserData *parser_data);
void parse_statement_list_(ParserData *parser_data);
void parse_statement(ParserData *parser_data);
void parse_statement_(ParserData *parser_data);
void parse_var(ParserData *parser_data);
void parse_var_(ParserData *parser_data);
void parse_expr_list(ParserData *parser_data);
void parse_expr_list_(ParserData *parser_data);
void parse_expr(ParserData *parser_data);
void parse_expr_(ParserData *parser_data);
void parse_simple_expr(ParserData *parser_data);
void parse_simple_expr_(ParserData *parser_data);
void parse_term(ParserData *parser_data);
void parse_term_(ParserData *parser_data);
void parse_factor(ParserData *parser_data);
void parse_factor_(ParserData *parser_data);
void parse_sign(ParserData *parser_data);

void synch(Production prod, MachineResult *tok, ParserData *parser_data);
void synerr(TokenType *expected, int len, MachineResult *found, ParserData *parser_data);

#endif