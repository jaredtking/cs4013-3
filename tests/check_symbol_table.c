#include "check_symbol_table.h"

START_TEST (test_check_enter_method)
{
	Symbol *result;

	result = check_enter_method("hey", parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("hey", result->name);
	ck_assert(result->type == PGNAME);

	result = check_enter_method("test", parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("test", result->name);
	ck_assert(result->type == FNAME);
}
END_TEST

START_TEST (test_check_exit_method)
{
	ck_assert(check_enter_method("program", parser_data) != NULL);

	ck_assert(check_enter_method("checkenter1", parser_data) != NULL);

	ck_assert_int_eq(0, check_exit_method(parser_data));

	ck_assert(check_enter_method("checkenter2", parser_data) != NULL);

	ck_assert(check_enter_method("checkenter3", parser_data) != NULL);

	ck_assert_int_eq(0, check_exit_method(parser_data));

	ck_assert_int_eq(0, check_exit_method(parser_data));
}
END_TEST

START_TEST (test_check_add_var)
{
	ck_assert(check_enter_method("checkaddvar", parser_data) != NULL);

	Symbol *result;

	result = check_add_var("int", INT, parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("int", result->name);

	result = check_add_var("real", REAL, parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("real", result->name);

	ck_assert(check_exit_method(parser_data) == 0);
}
END_TEST

START_TEST (test_check_add_prog_param)
{
	ck_assert(check_enter_method("checkaddparam", parser_data) != NULL);

	Symbol *result;

	result = check_add_prog_param("param", parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("param", result->name);

	ck_assert(check_exit_method(parser_data) == 0);
}
END_TEST

START_TEST (test_check_add_fun_param)
{
	ck_assert(check_enter_method("checkaddfparam", parser_data) != NULL);

	Symbol *result;

	result = check_add_fun_param("fparam", INT, parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("fparam", result->name);

	ck_assert(check_exit_method(parser_data) == 0);
}
END_TEST

START_TEST (test_get_symbol)
{
	SymbolTable *result = get_symbol("notfound", parser_data, 1);
	ck_assert(result == NULL);

	// program
	ck_assert(check_enter_method("program", parser_data) != NULL);

	result = get_symbol("program", parser_data, 1);
	ck_assert_str_eq("program", result->symbol->name);
	ck_assert(result->symbol->type == PGNAME);

	// program.pvar
	ck_assert(check_add_var("pvar", REAL, parser_data) != NULL);	

	// program.fun
	ck_assert(check_enter_method("fun", parser_data) != NULL);

	result = get_symbol("fun", parser_data, 1);
	ck_assert_str_eq("fun", result->symbol->name);
	ck_assert(result->symbol->type == FNAME);

	// check global search
	ck_assert(get_symbol("pvar", parser_data, 0) == NULL);

	result = get_symbol("pvar", parser_data, 1);
	ck_assert_str_eq("pvar", result->symbol->name);
	ck_assert(result->symbol->type == REAL);

	// exit program.fun
	ck_assert(check_exit_method(parser_data) == 0);

	// exit program
	ck_assert(check_exit_method(parser_data) == 0);
}
END_TEST

/*
	program
	  pvar
	  fun
	    -- search
*/

Suite * symbol_table_suite (void)
{
	Suite *s = suite_create ("Symbol Table");

	/* Core test case */
	TCase *tc_core = tcase_create ("Core");

	// initalize symbol table
	parser_data = (ParserData *)malloc(sizeof(ParserData));
	parser_data->symbol_table = NULL;
	parser_data->sym_eye = NULL;

	tcase_add_test (tc_core, test_check_enter_method);
	tcase_add_test (tc_core, test_check_exit_method);
	tcase_add_test (tc_core, test_check_add_var);
	tcase_add_test (tc_core, test_check_add_prog_param);
	tcase_add_test (tc_core, test_check_add_fun_param);
	tcase_add_test (tc_core, test_get_symbol);

	suite_add_tcase (s, tc_core);

	return s;
}