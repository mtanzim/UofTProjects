#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <check.h>
#include <sys/wait.h>
#include <errno.h>
#include "storage.h"

#define SERVEREXEC	"./server"	// Server executable file.
#define SERVEROUT	"server.out"	// File where the server's output is stored.
#define SERVEROUT_MODE	0666		// Permissions of the server ouptut file.
#define ONETABLE_CONF	"onetable.conf"	// Server configuration file with one table.
#define TWOTABLE_CONF	"twotables.conf"// Server configuration file with two tables.
#define KEY		"somekey"	// A key used in the test cases.
#define VALUE		"somevalue"	// A vaule used in the test cases.
#define TESTTIMEOUT	10		// How long to wait for each test to run.

// These settings should correspond to what's in the config file.
#define SERVERHOST	"localhost"	// The hostname where the server is running.
#define SERVERPORT	8888		// The port where the server is running.
#define TABLE1NAME	"foo"		// The first table to use.
#define TABLE2NAME	"bar"		// The second table to use.

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

/** 
 * @brief This test interacts with a server started with one table.
 *
 * It sets and gets a value on the first table (which should succeed), and 
 * gets * a value from a non- existent table (which should return with an 
 * error).
 */
START_TEST (test_setget1)
{
	// Start the server.
	int serverpid = start_server(ONETABLE_CONF); fail_if(serverpid <= 0,
			"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUE, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUE) != 0, "Got wrong value.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

/** 
 * @brief This test interacts with a server started with two tables.
 *
 * It sets and gets a value on the second table (which should succeed), and
 * gets a value from the first table (which should also succeed).  The test
 * assumes that test_setget1 was called first.
 */
START_TEST (test_setget2)
{
	// Start the server.
	int serverpid = start_server(TWOTABLE_CONF); fail_if(serverpid <= 0,
			"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value in the second table.
	struct storage_record r;
	strncpy(r.value, VALUE, sizeof r.value);
	int status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUE) != 0, "Got wrong value.");

	// Get a value from the first table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value from the first table.");
	fail_if(strcmp(r.value, VALUE) != 0, "Got wrong value from first table.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST


int main(int argc, char *argv[])
{
	// Create the test suite.
	// Note that each test starts the storage server, and kills it 
	// (automatically) at the end of each test.
	Suite *s = suite_create("changetables");
	TCase *tc = tcase_create("testcase");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_test(tc, test_setget1);
	///printf ("test 1 passed.\n");
	tcase_add_test(tc, test_setget2);
	///printf ("test 2 passed.\n");
	tcase_add_test(tc, test_setget1);
	///printf ("test 3 passed.\n");

	suite_add_tcase(s, tc);

	// Run the test suite.
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);
	return EXIT_SUCCESS;
}
