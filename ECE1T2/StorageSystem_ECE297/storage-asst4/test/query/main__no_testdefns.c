#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <check.h>
#include <sys/wait.h>
#include <errno.h>
#include "storage.h"
#include "queryconstants.h"

/**
 * @brief Start the storage server.
 *
 * @param config_file The configuration file the server should use.
 * @return Return server process id on success, or -1 otherwise.
 */
int start_server(char *config_file)
{
	pid_t childpid = fork();
	if (childpid < 0) {
		// Failed to create child.
		return -1;
	} else if (childpid == 0) {
		// The child.

		// Redirect stdout and stderr to a file.
		int outfd = creat(SERVEROUT, SERVEROUT_MODE);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		if (dup2(outfd, STDOUT_FILENO) < 0 || dup2(outfd, STDERR_FILENO) < 0) {
			perror("dup2 error");
			return -1;
		}

		// Start the server
		execl(SERVEREXEC, SERVEREXEC, config_file, NULL);

		// Should never get here.
		perror("Couldn't start server");
		exit(EXIT_FAILURE);
	} else {
		// The parent.

		// If the child terminates quickly, then there was probably a
		// problem running the server (e.g., config file not found).
		sleep(1);
		int pid = waitpid(childpid, NULL, WNOHANG);
		if (pid == childpid)
			return -1; // Probably a problem starting the server.
		else
			return childpid; // Probably ok.
	}
}

int main(int argc, char *argv[])
{
	// Create the test suite.
	// Note that each test starts the storage server, and kills it 
	// (automatically) at the end of each test.

/*###################  tests 1-6. VALID tests: SPACES and OPERATORS   #####################*/
	printf("\n");
	Suite *s1 = suite_create("ALL valid");

//####	testing SPACES and OPERATORS
	printf("\n");
	TCase *tc1 = tcase_create("testing SPACES and OPERATORS");
	tcase_set_timeout(tc1, TESTTIMEOUT);
	

	//	table 1 and 1 query call using 			"stops > 10". 
	//	Want 1 of 3 matches stored in keys_found.
	tcase_add_test(tc1, testquery1);	
		
	//	table 1 and 1 query call using 			"stops> 10".
	//	Want 2 of 3 matches stored in keys_found.
	tcase_add_test(tc1, testquery2);	

	//	table 1 and 1 query call using 			"kilometres <40.0".
	//	Want 2 of 2 matches stored in keys_found.
	tcase_add_test(tc1, testquery3);	

	//	table 1 and 1 query call using			"name=some station"
	//	Want 1 of 1 match stored in keys_found.
	tcase_add_test(tc1, testquery4);	

	//	table 1 and 1 query call using			
	//	Want 1 of 1 matches stored in keys_found for	"stops=16, kilometres  > 60"
	tcase_add_test(tc1, testquery5);	

	suite_add_tcase(s1, tc1);

//####	testing OTHER TABLES
	printf("\n");
	TCase *tc2 = tcase_create("testing OTHER TABLES");
	tcase_set_timeout(tc2, TESTTIMEOUT);

	//	table 2 with 5 query calls		
	//	Want 2 of 3 matches for "highTemperature > +1.0"
	//	Want 4 of 4 matches for "lowTemperature >-300.3"
	//	Want 1 of 1 matches for "province=province3, highTemperature> +36.1"
	//	Want 1 of 1 matches for "province=province3, highTemperature< 10000.1 , lowTemperature =-242.9"
	//	Want 0 of 1 matches for	"province=province3, highTemperature< 10000.1 , lowTemperature > -242.9"
	tcase_add_test(tc2, testquery6);	

	suite_add_tcase(s1, tc2);

//####	testing MULTIPLE TABLES
	printf("\n");
	TCase *tc3 = tcase_create("testing MULTIPLE TABLES");
	tcase_set_timeout(tc3, TESTTIMEOUT);
	
	//	table 3 with 1 query call	
	//	Want 1 of 3 matches for		"brand =BMW"
	//	table 4 with 2 query calls
	//	Want 2 of 2 matches for 	"Grade <  66.0" 
	//	Want 1 of 2 matches for 	"id>127"
	tcase_add_test(tc3, testquery7);

	suite_add_tcase(s1, tc3);
//	Run the test suite.
	SRunner *sr = srunner_create(s1);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

/*###################  tests 8-?. INVALID tests: SPACES and OPERATORS   #####################*/
	printf("\n");
	Suite *s2 = suite_create("ALL INVALID: SPACES and OPERATORS");
	TCase *tc4 = tcase_create("testing SPACES and OPERATORS");
	tcase_set_timeout(tc4, TESTTIMEOUT);

	// table 1 with 1 query call, invalid input	"stops 31"
	// table 3 with 1 query call, valid input (look at testquery7)
	tcase_add_test(tc4, testquery8);

	// table 2 with 1 query call, valid input 
	//	Want 2 of 3 matches for "highTemperature > +1.0"
	// table 3 with 2 query calls, 2 predicates (1 invalid) "stops=16, kilometres  >= 60"
	tcase_add_test(tc4, testquery9);

	sr = srunner_create(s2);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

//####	Other tests

	//check for non-existing values
	//check for want match more than actual match found
	//check for want match more than values stored

	return EXIT_SUCCESS;
}
