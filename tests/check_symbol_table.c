#include "check_symbol_table.h"

START_TEST (test_symbol_table)
{

}
END_TEST

Suite * symbol_table_suite (void)
{
	Suite *s = suite_create ("Symbol Table");

	/* Core test case */
	TCase *tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_symbol_table);

	suite_add_tcase (s, tc_core);

	return s;
}