#include <stdlib.h>
#include <check.h>

#define TESTTIMEOUT	10		// Time to wait for each test to run.


/**
 * This test makes sure that the storage.h file has not been modified.
 */
START_TEST (test_filemod)
{
	int status = system("md5sum --status -c md5sum.check &> /dev/null");
	fail_if(status == 1, "storage.h has been modified.");
	fail_if(status != 0, "Error computing md5sum of storage.h.");
}
END_TEST


int main(int argc, char *argv[])
{
	// Create the test suite.
	Suite *s = suite_create("filemod");
	TCase *tc = tcase_create("testcase");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_test(tc, test_filemod);
	suite_add_tcase(s, tc);

	// Run the test suite.
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);
	return EXIT_SUCCESS;
}
