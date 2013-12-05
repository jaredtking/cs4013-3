#include "check_symbol_table.h"

START_TEST (test_check_add_var)
{
	check_add_var("test", FNAME, parser_data);
}
END_TEST

Suite * symbol_table_suite (void)
{
	Suite *s = suite_create ("Symbol Table");

	/* Core test case */
	TCase *tc_core = tcase_create ("Core");

	// initalize symbol table
	parser_data = (ParserData *)malloc(sizeof(ParserData));
	parser_data->symbol_table = (SymbolTable *)malloc(sizeof(SymbolTable));
	parser_data->symbol_table->symbol = NULL;
	parser_data->symbol_table->next = NULL;

	tcase_add_test (tc_core, test_check_add_var);

	suite_add_tcase (s, tc_core);

	return s;
}