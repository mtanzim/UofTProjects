/**
 * @file
 * @brief test code for storage_query function.
 *
 * @author Wina Ng
 */

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
 * @author Wina Ng
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

START_TEST (testquery1)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for stops > 10.
	// There should be 3 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "stops > 10", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == 3, "Query should find 3 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be either KEY1 or KEY3 or KEY4.
	fail_unless(
		(
			(strcmp(keys_found[0], KEY1) == 0)|| 
			(strcmp(keys_found[0], KEY3) == 0)|| 
			(strcmp(keys_found[0], KEY4) == 0)
		), 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}


//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery2)//table 1 and 1 query call using "stops> 10".Want 1 match stored in keys_found.
{
//####	INITIALIZATION. ## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "stops> 10".
	// There should be 3 matches, but ask for only 2 results.
	int matches = storage_query(TABLE1NAME, "stops>10", keys_found, 2, conn);
	fail_unless(matches == 3, "Query should find 3 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching keys should be two of: KEY1, KEY3, or KEY4".
	fail_unless(
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY3) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY3) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY4) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY4) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY3) == 0) && (strcmp(keys_found[1], KEY4) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY4) == 0) && (strcmp(keys_found[1], KEY3) == 0)
			), 
		"The returned keys do not match the query.\n");

	// Make sure the third key is not set to anything.
	fail_unless(keys_found[2][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery3)
{
//####	INITIALIZATION. ## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "kilometres <40.0".
	// There should be 2 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "kilometres <40.0", keys_found, 2, conn);
	fail_unless(matches == 2, "Query should find 2 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The first matching key should be either KEY1 or KEY2. 2nd key must be the other.
	fail_unless(
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY2) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY2) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			), 
		"The returned keys do not match the query.\n");

	// Make sure the third key is not set to anything.
	fail_unless(keys_found[2][0] == '\0', "There should only be two keys returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery4)
{
//####	INITIALIZATION. ## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "name=some station".
	// There should be 1 match only.
	int matches = storage_query(TABLE1NAME, "name=some station", keys_found, 1, conn);
	fail_unless(matches == 1, "Query should find 1 matching record.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should KEY3.
	fail_unless(
			(
			strcmp(keys_found[0], KEY3) == 0
			), 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery5)
{
	//	table 1 and 1 query call		
	//	Want 1 of 1 matches stored in keys_found for	"stops=16, kilometres  > 60"

//####	INITIALIZATION. ## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	//####	QUERY CALL - "stops=16, kilometres  > 60"

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "stops=16, kilometres  > 60".
	// There should be only 1 match.
	int matches = storage_query(TABLE1NAME, "stops=16, kilometres  > 60", keys_found, 1, conn);
	fail_unless(matches == 1, "Query should find 1 matching record.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be KEY3.
	fail_unless(
			(
			strcmp(keys_found[0], KEY3) == 0
			), 
		"The returned keys do not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST


START_TEST (testquery6)
{
	//	table 2 with 5 query calls	
	
	//	Want 2 of 3 matches for 	"highTemperature > +1.0"
	//	Want 4 of 4 matches for 	"lowTemperature >-300.3"
	//	Want 1 of 1 matches for 	"province=province3, highTemperature> +36.1"
	//	Want 1 of 1 matches for 	"province=province3, highTemperature< 10000.1 , lowTemperature =-242.9"
	//	Want 0 of 1 matches for 	"province=province3, highTemperature< 10000.1, lowTemperature > -242.9"

//####	INITIALIZATION. ## No need to modify this part unless you use a different table.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 10;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE2VAL1, sizeof r.value);
	int status = storage_set(TABLE2NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE2VAL2, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE2VAL3, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE2VAL4, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL #1 - "highTemperature >+1.0"
	//	Want 2 of 3 matches

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "highTemperature >+1.0".
	// There should be 3 matches. but only want 2 keys returned.
	int matches = storage_query(TABLE2NAME, "highTemperature >+1.0", keys_found, 2, conn);
	fail_unless(matches == 3, "Query should find 3 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching keys should be two of: KEY1, KEY3, KEY4.
	fail_unless(
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY3) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY3) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY4) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY3) == 0) && (strcmp(keys_found[1], KEY4) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) && (strcmp(keys_found[1], KEY3) == 0)
			), 
		"The returned keys do not match the query.\n");

	// Make sure the third key is not set to anything.
	fail_unless(keys_found[2][0] == '\0', "There should only be two keys returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	QUERY CALL #2 - "lowTemperature >-300.3", Want 4 of 4 matches

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "lowTemperature >-300.3".
	// There should be 4 matches. want 4 keys returned.
	matches = storage_query(TABLE2NAME, "lowTemperature >-300.3", keys_found, 4, conn);
	fail_unless(matches == 4, "Query should find 4 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching keys should be KEY1, KEY2, KEY3 or KEY4 in *any* order.
	fail_unless((
			(
				(strcmp(keys_found[0], KEY1) == 0) &&
				(strcmp(keys_found[1], KEY2) == 0) &&
				(strcmp(keys_found[2], KEY3) == 0) &&
				(strcmp(keys_found[3], KEY4) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY1) == 0) &&
				(strcmp(keys_found[1], KEY2) == 0) &&
				(strcmp(keys_found[2], KEY4) == 0) &&
				(strcmp(keys_found[3], KEY3) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY1) == 0) &&
				(strcmp(keys_found[1], KEY3) == 0) &&
				(strcmp(keys_found[2], KEY2) == 0) &&
				(strcmp(keys_found[3], KEY4) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY1) == 0) &&
				(strcmp(keys_found[1], KEY3) == 0) &&
				(strcmp(keys_found[2], KEY4) == 0) &&
				(strcmp(keys_found[3], KEY2) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY1) == 0) &&
				(strcmp(keys_found[1], KEY4) == 0) &&
				(strcmp(keys_found[2], KEY2) == 0) &&
				(strcmp(keys_found[3], KEY3) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY1) == 0) &&
				(strcmp(keys_found[1], KEY4) == 0) &&
				(strcmp(keys_found[2], KEY3) == 0) &&
				(strcmp(keys_found[3], KEY2) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY2) == 0) &&
				(strcmp(keys_found[1], KEY1) == 0) &&
				(strcmp(keys_found[2], KEY3) == 0) &&
				(strcmp(keys_found[3], KEY4) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY2) == 0) &&
				(strcmp(keys_found[1], KEY1) == 0) &&
				(strcmp(keys_found[2], KEY4) == 0) &&
				(strcmp(keys_found[3], KEY3) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY2) == 0) &&
				(strcmp(keys_found[1], KEY3) == 0) &&
				(strcmp(keys_found[2], KEY1) == 0) &&
				(strcmp(keys_found[3], KEY4) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY2) == 0) &&
				(strcmp(keys_found[1], KEY3) == 0) &&
				(strcmp(keys_found[2], KEY4) == 0) &&
				(strcmp(keys_found[3], KEY1) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY2) == 0) &&
				(strcmp(keys_found[1], KEY4) == 0) &&
				(strcmp(keys_found[2], KEY1) == 0) &&
				(strcmp(keys_found[3], KEY3) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY2) == 0) &&
				(strcmp(keys_found[1], KEY4) == 0) &&
				(strcmp(keys_found[2], KEY3) == 0) &&
				(strcmp(keys_found[3], KEY1) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) &&
				(strcmp(keys_found[1], KEY1) == 0) &&
				(strcmp(keys_found[2], KEY2) == 0) &&
				(strcmp(keys_found[3], KEY3) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) &&
				(strcmp(keys_found[1], KEY1) == 0) &&
				(strcmp(keys_found[2], KEY3) == 0) &&
				(strcmp(keys_found[3], KEY2) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) &&
				(strcmp(keys_found[1], KEY2) == 0) &&
				(strcmp(keys_found[2], KEY1) == 0) &&
				(strcmp(keys_found[3], KEY3) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) &&
				(strcmp(keys_found[1], KEY2) == 0) &&
				(strcmp(keys_found[2], KEY3) == 0) &&
				(strcmp(keys_found[3], KEY1) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) &&
				(strcmp(keys_found[1], KEY3) == 0) &&
				(strcmp(keys_found[2], KEY1) == 0) &&
				(strcmp(keys_found[3], KEY2) == 0) 
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) &&
				(strcmp(keys_found[1], KEY3) == 0) &&
				(strcmp(keys_found[2], KEY2) == 0) &&
				(strcmp(keys_found[3], KEY1) == 0) 
			)), 
		"The returned keys do not match the query.\n");

	// Make sure the other keys are not set to anything.
	for (i = 4; i < num_keys_found; i++)
	{
		fail_unless(keys_found[i][0] == '\0', "There should only be four keys returned.\n");
	}

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	QUERY CALL #3 - Want 1 of 1 matches for	"province=province3, highTemperature> +36.1"

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "province=province3, highTemperature> +36.1."
	// There should be only 1 match.
	matches = storage_query(TABLE2NAME, "province=province3, highTemperature> +36.1", keys_found, 1, conn);
	fail_unless(matches == 1, "Query should find 1 matching record.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching keys should be KEY3.
	fail_unless(
			(
			strcmp(keys_found[0], KEY3) == 0 
			), 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	QUERY CALL #4 - Want 1 of 1 matches for "province=province3, highTemperature< 10000.1 , lowTemperature =-242.9"
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "province=province3, highTemperature> +36.1."
	// There should be only 1 match.
	matches = storage_query(TABLE2NAME, "province=province3, highTemperature< 10000.1 , lowTemperature =-242.9", keys_found, 1, conn);
	fail_unless(matches == 1, "Query should find 1 matching record.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching keys should be KEY3.
	fail_unless(
			(
			strcmp(keys_found[0], KEY3) == 0 
			), 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	QUERY CALL #5: Want 0 of 1 matches for "province=province3, highTemperature< 10000.1, lowTemperature > -242.9"
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "province=province3, highTemperature< 10000.1, lowTemperature > -242.9"
	// There should be only 1 match.
	matches = storage_query(TABLE2NAME, "province=province3, highTemperature< 10000.1, lowTemperature > -242.9", keys_found, 1, conn);
	fail_unless(matches == 1, "Query should find 1 matching record.");
	
	// There should no key returned.
	fail_unless(keys_found[0][0] == '\0', "There should no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery7)
{
	//	table 3 with 1 query call	
	//	Want 1 of 3 matches for		"brand =BMW"
	//	table 4 with 2 query calls
	//	Want 2 of 2 matches for 	"Grade <  66.0" 
	//	Want 1 of 3 matches for 	"id>127"

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE3VAL1, sizeof r.value);
	int status = storage_set(TABLE3NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL2, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL3, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL4, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL#1 - Want 1 of 3 matches for		"brand =BMW"
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "brand =BMW".
	// There should be 3 matches, but ask for only one result.
	int matches = storage_query(TABLE3NAME, "brand =BMW", keys_found, 1, conn); //1=(max_keys_want) want # returned
	fail_unless(matches == 3, "Query should find 3 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be either "first" or "third" or "fourth".
	fail_unless(
		(
			(strcmp(keys_found[0], "first") == 0)|| 
			(strcmp(keys_found[0], "third") == 0)|| 
			(strcmp(keys_found[0], "fourth") == 0)
		), 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}


//####	TABLE4.

	// Store an item.
	strncpy(r.value, TABLE4VAL1, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL2, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL3, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL4, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL #2 - Want 2 of 2 matches for "Grade <  66.0" 
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "Grade <  66.0" .
	// There should be 2 matches, but ask for two results.
	matches = storage_query(TABLE4NAME, "Grade <  66.0", keys_found, 2, conn);
	fail_unless(matches == 2, "Query should find 2 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching keys should be KEY2 and KEY4, in any order.
	fail_unless(
			(
				(
					(strcmp(keys_found[0], KEY2) == 0) &&
					(strcmp(keys_found[1], KEY4) == 0)	
				)
			|| 
				(
					(strcmp(keys_found[0], KEY4) == 0) &&
					(strcmp(keys_found[1], KEY2) == 0)
				)
			), 
			"The returned keys do not match the query.\n"
		   );

	// Make sure the third key is not set to anything.
	fail_unless(keys_found[2][0] == '\0', "There should only be two keys returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	QUERY CALL #3 - Want 1 of 2 matches for 	"id>127, grade > 60.0" 
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "id>127, grade > 60.0"  .
	// There should be 2 matches, but ask for 1 result.
	matches = storage_query(TABLE4NAME, "id>127, grade > 60.0" , keys_found, 1, conn);
	fail_unless(matches == 2, "Query should find 2 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be either KEY1 or KEY3.
	fail_unless(
			(
				(strcmp(keys_found[0], KEY1) == 0)||
				(strcmp(keys_found[0], KEY3) == 0)
			), 
			"The returned key does not match the query.\n"
		   );

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}


//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery8)
{
	// table 1 with 1 query call, invalid input	"stops 31"
	// table 3 with 1 query call, valid input (look at testquery7)

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");


//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "stops 31".
	// There should be -1 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "stops 31", keys_found, 1, conn); //1=(max_keys_want) want # returned
	fail_unless(matches == -1, "Query should fail processing invalid input.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	TABLE 3 INITIALIZATION	## Adapted from testquery7. No need to modify this part.##
//store into table 3.

	// Store an item.
	strncpy(r.value, TABLE3VAL1, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL2, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL3, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL4, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL#1 - Want 1 of 3 matches for		"brand =BMW"
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "brand =BMW".
	// There should be 3 matches, but ask for only one result.
	matches = storage_query(TABLE3NAME, "brand =BMW", keys_found, 1, conn); //1=(max_keys_want) want # returned
	fail_unless(matches == 3, "Query should find 3 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be either KEY1 or KEY3 or KEY4.
	fail_unless(
		(
			(strcmp(keys_found[0], KEY1) == 0)|| 
			(strcmp(keys_found[0], KEY3) == 0)|| 
			(strcmp(keys_found[0], KEY4) == 0)
		), 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}


//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST


START_TEST (testquery9)
{
	// table 2 with 1 query call, valid input 
	//	Want 2 of 3 matches for 	"highTemperature > +1.0" (reger to testquery 6)
	// table 3 with 2 query calls, 
	// 	2 predicates (1 invalid) 	"stops=16, kilometres  >= 60"
	//	Want 1 of 3 matches for		"brand =BMW" (refer to testquery 7)

//####	INITIALIZATION. ## No need to modify this part unless you use a different table.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 10;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE2VAL1, sizeof r.value);
	int status = storage_set(TABLE2NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE2VAL2, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE2VAL3, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE2VAL4, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL #1 - "highTemperature >+1.0"
	//	Want 2 of 3 matches

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "highTemperature >+1.0".
	// There should be 3 matches. but only want 2 keys returned.
	int matches = storage_query(TABLE2NAME, "highTemperature >+1.0", keys_found, 2, conn);
	fail_unless(matches == 3, "Query should find 3 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching keys should be two of: KEY1, KEY3, KEY4.
	fail_unless(
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY3) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY3) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY4) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY3) == 0) && (strcmp(keys_found[1], KEY4) == 0)
			)
		   ||
			(
				(strcmp(keys_found[0], KEY4) == 0) && (strcmp(keys_found[1], KEY3) == 0)
			), 
		"The returned keys do not match the query.\n");

	// Make sure the third key is not set to anything.
	fail_unless(keys_found[2][0] == '\0', "There should only be two keys returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}
//####	Table 3 with 2 query calls
//####	Storing into table 3

// Store an item.
	strncpy(r.value, TABLE3VAL1, sizeof r.value);
	storage_set(TABLE3NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL2, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL3, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL4, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL#2 - 2 predicates (1 invalid) 	"stops=16, kilometres  >= 60"
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "stops=16, kilometres  >= 60".
	// Should fail.
	matches = storage_query(TABLE3NAME, "stops=16, kilometres  >= 60", keys_found, 1, conn);
	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	QUERY CALL #3
//	Want 1 of 3 matches for		"brand =BMW"
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "brand =BMW".
	// There should be 3 matches, but ask for only one result.
	matches = storage_query(TABLE3NAME, "brand =BMW", keys_found, 1, conn); 
	fail_unless(matches == 3, "Query should find 3 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be either KEY1 or KEY3 or KEY4.
	fail_unless(
		(
			(strcmp(keys_found[0], KEY1) == 0)|| 
			(strcmp(keys_found[0], KEY3) == 0)|| 
			(strcmp(keys_found[0], KEY4) == 0)
		), 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery10)
{
	//table 4 with 1 query call, invalid input "id 127, Grade 55.0"

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE4VAL1, sizeof r.value);
	int status = storage_set(TABLE4NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL2, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL3, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL4, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - "id 127, Grade 55.0"
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "id 127, Grade 55.0".
	// should fail.
	int matches = storage_query(TABLE4NAME, "id 127, Grade 55.0", keys_found, 3, conn);
	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery11)
{
	// Refer to testquery3
	//	table 1 and 1 query call using 			"kilometres <40.0".
	//	Want 4 of 2 matches stored in keys_found.

//####	INITIALIZATION. ## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "kilometres <40.0".
	// There should be 2 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "kilometres <40.0", keys_found, 4, conn);
	fail_unless(matches == 2, "Query should find 2 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The first matching key should be either KEY1 or KEY2. 2nd key must be the other.
	fail_unless(
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY2) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY2) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			), 
		"The returned keys do not match the query.\n");

	// Make sure the third key is not set to anything.
	fail_unless(keys_found[2][0] == '\0', "There should only be two keys returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery12)
{
	// Refer to testquery3
	//	table 1 and 1 query call using 			"kilometres <40.0".
	//	Want 9 of 2 matches stored in keys_found.

//####	INITIALIZATION. ## No need to modify this part.##
	struct storage_record r;

	// Create an array of 8 strings.
	const int num_keys_found = 8;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "kilometres <40.0".
	// There should be 2 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "kilometres <40.0", keys_found, 9, conn);
	fail_unless(matches == 2, "Query should find 2 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The first matching key should be either KEY1 or KEY2. 2nd key must be the other.
	fail_unless(
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY2) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY2) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			), 
		"The returned keys do not match the query.\n");

	// Make sure the third key is not set to anything.
	fail_unless(keys_found[2][0] == '\0', "There should only be two keys returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery13)
{
	// Refer to testquery3
	//	table 1 and 1 query call using 			"kilometres <40.0".
	//	Want 5 of 2 matches stored in keys_found.

//####	INITIALIZATION. ## No need to modify this part.##
	struct storage_record r;

	// Create an array of 8 strings.
	const int num_keys_found = 8;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "kilometres <40.0".
	// There should be 2 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "kilometres <40.0", keys_found, 5, conn);
	fail_unless(matches == 2, "Query should find 2 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The first matching key should be either KEY1 or KEY2. 2nd key must be the other.
	fail_unless(
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY2) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY2) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			), 
		"The returned keys do not match the query.\n");

	// Make sure the third key is not set to anything.
	fail_unless(keys_found[2][0] == '\0', "There should only be two keys returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery14)
{
	//table 1 with 1 query call, "name=anotherstation"

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "name=anotherstation".
	// There should be 3 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "name=anotherstation", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == -1, "Query should find no matching records.Check your whitespace parser.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST
START_TEST (testquery15)
{
	//table 1 with 1 query call, "name=another station,,"

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "name=another station,,".
	// There should be 3 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "name=another station,,", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == -1, "Query should find no matching records.Check your commas parser.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery16) 

//#########
//######### IMPORTANT #########: Check with Tanzim's code if it does what it's supposed to do!
//#########
{
	// Refer to testquery3
	//	table 1 and 1 query call using 			"kilometres <40.0".
	//	Want -2 of 2 matches stored in keys_found.

//####	INITIALIZATION. ## No need to modify this part.##
	struct storage_record r;

	// Create an array of 8 strings.
	const int num_keys_found = 8;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "kilometres <40.0".
	// There should be 2 matches, but ask for -2 result.
	int matches = storage_query(TABLE1NAME, "kilometres <40.0", keys_found, -2, conn);
	fail_unless(matches == 2, "Query should find 2 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// should not assign any values to keys_found array.
	fail_if(
			(
				(strcmp(keys_found[0], KEY1) == 0) && (strcmp(keys_found[1], KEY2) == 0)
			)
		    || //or
			(
				(strcmp(keys_found[0], KEY2) == 0) && (strcmp(keys_found[1], KEY1) == 0)
			), 
		"The returned keys should not match the query because of invalid # of matches wanted.\n");

	// Make sure the third key is not set to anything.
	fail_unless(keys_found[2][0] == '\0', "There should only no keys returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery17)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "lowTemperature <-242.9".
	// There should be 0 matches
	int matches = storage_query(TABLE1NAME, "lowTemperature <-242.9", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == 0, "Query should find 0 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery18)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "lowTemperature <-242.9, stops > 3".
	// There should be 0 matches
	int matches = storage_query(TABLE1NAME, "lowTemperature <-242.9, stops > 3", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == 0, "Query should find 0 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery19)
{
	//table 4 with 1 query call, invalid input "id 127 Grade      55.0   "

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE4VAL1, sizeof r.value);
	int status = storage_set(TABLE4NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL2, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL3, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL4, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - "id 127, Grade 55.0"
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "id 127, Grade 55.0".
	// should fail.
	int matches = storage_query(TABLE4NAME, "id 127 Grade      55.0   ", keys_found, 3, conn);
	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery20)
{
	//table 4 with 1 query call, invalid input "id 127 Grade      55.0  , "

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE4VAL1, sizeof r.value);
	int status = storage_set(TABLE4NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL2, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL3, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL4, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - "id 127 Grade      55.0  , "
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "id 127 Grade      55.0  , ".
	// should fail.
	int matches = storage_query(TABLE4NAME, "id 127 Grade      55.0  , ", keys_found, 3, conn);
	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery21)
{
	//table 4 with 1 query call, invalid input ",,id 127 Grade      55.0   "

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE4VAL1, sizeof r.value);
	int status = storage_set(TABLE4NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL2, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL3, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL4, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - ",,id 127 Grade      55.0   "
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for ",,id 127 Grade      55.0   ".
	// should fail.
	int matches = storage_query(TABLE4NAME, ",,id 127 Grade      55.0   ", keys_found, 3, conn);
	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery22)
{
	//table 4 with 1 query call, invalid input "    id   127   Grade 55.0   "

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE4VAL1, sizeof r.value);
	int status = storage_set(TABLE4NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL2, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL3, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL4, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - "    id   127   Grade 55.0   "
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "    id   127   Grade 55.0   ".
	// should fail.
	int matches = storage_query(TABLE4NAME, "    id   127   Grade 55.0   ", keys_found, 3, conn);
	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery23)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for stops > 10.
	// There should be 3 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "stops > 10", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == 3, "Query should find 3 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be either KEY1 or KEY3 or KEY4.
	fail_unless(
		(
			(strcmp(keys_found[0], KEY1) == 0)|| 
			(strcmp(keys_found[0], KEY3) == 0)|| 
			(strcmp(keys_found[0], KEY4) == 0)
		), 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Store into Table 2.
	// Store an item.
	strncpy(r.value, TABLE2VAL1, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE2VAL2, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE2VAL3, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE2VAL4, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL for Table 2.

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "name = Kipling".
	// should fail.
	matches = storage_query(TABLE2NAME, "name = Kipling", keys_found, 1, conn);
	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	QUERY CALL for Table 4.

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "name = Kipling".
	// should fail.
	matches = storage_query(TABLE4NAME, "name = Kipling", keys_found, 1, conn);
	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery24)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "".
	// There should be 0 matches
	int matches = storage_query(TABLE1NAME, "", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == 0, "Query should find 0 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery25)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "stops = lalala".
	// There should be 0 matches
	int matches = storage_query(TABLE1NAME, "stops = lalala", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == 0, "Query should find 0 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery26)
{
	//	table 1 and 1 query call	(refer to testquery5)
	//	2 predicates not in order
	//	Want 1 of 1 matches stored in keys_found for	"kilometres  > 60 ,stops=16"

//####	INITIALIZATION. ## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	//####	QUERY CALL - "kilometres  > 60 ,stops=16"

	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "kilometres  > 60 ,stops=16".
	// There should be only 1 match.
	int matches = storage_query(TABLE1NAME, "kilometres  > 60 ,stops=16", keys_found, 1, conn);
	fail_unless(matches == 1, "Query should find 1 matching record.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be KEY3.
	fail_unless(
			(
			strcmp(keys_found[0], KEY3) == 0
			), 
		"The returned keys do not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery27)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "name = Kipling, stops = lalala , kilometres < 20.0".
	// There should be 0 matches
	int matches = storage_query(TABLE1NAME, "name = Kipling, stops = lalala , kilometres < 20.0", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == 0, "Query should find 0 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery28)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL #1 - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for stops > 10.
	// There should be 3 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "stops > 10", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == 3, "Query should find 3 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be either KEY1 or KEY3 or KEY4.
	fail_unless(
		(
			(strcmp(keys_found[0], KEY1) == 0)|| 
			(strcmp(keys_found[0], KEY3) == 0)|| 
			(strcmp(keys_found[0], KEY4) == 0)
		), 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Table 3 store

	// Store an item.
	strncpy(r.value, TABLE3VAL1, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL2, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL3, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL4, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL#2
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "brand < -3".
	// There should be 3 matches, but ask for only one result.
	matches = storage_query(TABLE3NAME, "brand < -3", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery29)
{
	//table 4 with 1 query call, invalid input "id 127 ,,,, ,  , Grade    55.0   "

//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE4VAL1, sizeof r.value);
	int status = storage_set(TABLE4NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL2, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL3, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE4VAL4, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - "id 127 ,,,, ,  , Grade    55.0   "
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "id 127 ,,,, ,  , Grade    55.0   ".
	// should fail.
	int matches = storage_query(TABLE4NAME, "id 127 ,,,, ,  , Grade    55.0   ", keys_found, 3, conn);
	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery30)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL #1 - DIFFERENT inputs and outputs for every test.
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "name=anotherstation".
	// There should be 3 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "name=anotherstation", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == -1, "Query should find no matching records.Check your whitespace parser.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only be no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Table 3 store

	// Store an item.
	strncpy(r.value, TABLE3VAL1, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL2, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL3, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE3VAL4, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL#2
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "brand < -3".
	// There should be 3 matches, but ask for only one result.
	matches = storage_query(TABLE3NAME, "brand < -3", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == -1, "Query should fail.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testquery31)
{
//####	INITIALIZATION	## No need to modify this part.##
	struct storage_record r;

	// Create an array of 4 strings.
	const int num_keys_found = 4;
	char *keys_found[num_keys_found];
	int i;

	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF);
	fail_unless(serverpid > 0, "Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Store an item.
	strncpy(r.value, TABLE1VAL1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY1, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL2, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL3, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY3, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

	// Store an item.
	strncpy(r.value, TABLE1VAL4, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY4, &r, conn);
	fail_unless(status == 0, "Error storing an item.");

//####	QUERY CALL - DIFFERENT inputs and outputs for every test.
	
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}

	// Query for "stops < ".
	// There should be 0 matches
	int matches = storage_query(TABLE1NAME, "stops < ", keys_found, 1, conn);
	//# before conn = (max_keys_want) = want # returned

	fail_unless(matches == 0, "Query should find 0 matching records.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// Make sure the key is not set to anything.
	fail_unless(keys_found[0][0] == '\0', "There should only no key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}

//####	Disconnect from the server. ## No need to modify this part.##
	status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST


int main(int argc, char *argv[])
{
	// Create the test suite.
	// Note that each test starts the storage server, and kills it 
	// (automatically) at the end of each test.

//##############################################################################
/*######  tests 1-7. VALID tests: SPACES and OPERATORS   #####################*/
	printf("\n");
	Suite *s1 = suite_create("Suite 1 - ALL valid");

//####	testing SPACES and OPERATORS
	printf("\n");
	TCase *tc1 = tcase_create("tests 1-5,26. testing VALID SPACES and OPERATORS");
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

	//	table 1 and 1 query call		
	//	Want 1 of 1 matches stored in keys_found for	"stops=16, kilometres  > 60"
	tcase_add_test(tc1, testquery5);	

	//	table 1 and 1 query call	(refer to testquery5)
	//	2 predicates not in order
	//	Want 1 of 1 matches stored in keys_found for	"kilometres  > 60 ,stops=16"
	tcase_add_test(tc1, testquery26);

	suite_add_tcase(s1, tc1);

//####	testing OTHER TABLES
	printf("\n");
	TCase *tc2 = tcase_create("test 6. testing VALID OTHER TABLES");
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
	TCase *tc3 = tcase_create("test 7. testing VALID MULTIPLE TABLES");
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
	printf("\n");

//##############################################################################
/*######  tests 8-10,14-15,19-22,29. "FAIL" tests: SPACES and OPERATORS   ########*/
	printf("\n");
	Suite *s2 = suite_create("Suite 2 - tests 8-10,14-15,19-22,29. ALL INVALID: SPACES and OPERATORS");
	TCase *tc4 = tcase_create("testing SPACES and OPERATORS");
	tcase_set_timeout(tc4, TESTTIMEOUT);

	// table 1 with 1 query call, invalid input	"stops 31"
	// table 3 with 1 query call, valid input (look at testquery7)
	tcase_add_test(tc4, testquery8);

	// table 2 with 1 query call, valid input 
	//	Want 2 of 3 matches for 	"highTemperature > +1.0" (refer to testquery 6)
	// table 3 with 2 query calls, 
	// 	2 predicates (1 invalid) 	"stops=16, kilometres  >= 60"
	//	Want 1 of 3 matches for		"brand =BMW" (refer to testquery 7)
	tcase_add_test(tc4, testquery9);	//should fail

	//table 4 with 1 query call, invalid input "id 127, Grade 55.0"
	// 2 invalid predicates
	tcase_add_test(tc4, testquery10);

	//checking whitespaces.
	//table 1 with 1 query call, "name=anotherstation"
	tcase_add_test(tc4, testquery14);

	//comma at the end
	//refer to testquery14
	//table 1 with 1 query call, "name=another station,,"
	tcase_add_test(tc4, testquery15);

	//no commas
	//refer to testquery 10
	//table 4 with 1 query call, invalid input "id 127 Grade      55.0   "
	// 2 invalid predicates
	tcase_add_test(tc4, testquery19);

	//refer to testquery 10
	//table 4 with 1 query call, invalid input "id 127 Grade      55.0  , "
	// 2 invalid predicates
	tcase_add_test(tc4, testquery20);
	
	//commas in front
	//refer to testquery 10
	//table 4 with 1 query call, invalid input ",,id 127 Grade      55.0   "
	// 2 invalid predicates
	tcase_add_test(tc4, testquery21);

	//too many spaces, no commas
	//refer to testquery 10
	//table 4 with 1 query call, invalid input "    id   127   Grade 55.0   "
	// 2 invalid predicates
	tcase_add_test(tc4, testquery22);

	//extra commas and spaces in middle
	//refer to testquery 10
	//table 4 with 1 query call, invalid input "id 127 ,,,, ,  , Grade    55.0   "
	// 2 invalid predicates
	tcase_add_test(tc4, testquery29);

	suite_add_tcase(s2, tc4);

	sr = srunner_create(s2);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);
	printf("\n");

//##############################################################################
/*########  tests 11-13,16. "VALID/INVALID" tests: Want # of matches  ###########*/
	printf("\n");
	Suite *s3 = suite_create("Suite 3 - tests 11-13,16. VALID/INVALID tests: Want # of matches");
	TCase *tc5 = tcase_create("testing with Want # of matches");
	tcase_set_timeout(tc5, TESTTIMEOUT);

	//check for want match more than # of values found/matched.
	// Refer to testquery3
	//	table 1 and 1 query call using 			"kilometres <40.0".
	//	Want *4* of 2 matches stored in keys_found.
	tcase_add_test(tc5, testquery11);	// should be valid.

	//check for want match more than # of keys allocated
	// Refer to testquery3
	//	table 1 and 1 query call using 			"kilometres <40.0".
	//	Want *9* of 2 matches stored in keys_found. 8 return keys are allocated.
	tcase_add_test(tc5, testquery12);	// should be valid.

	//check for want match more than # of values stored
	// Refer to testquery3
	//	table 1 and 1 query call using 			"kilometres <40.0".
	//	Want *5* of 2 matches stored in keys_found. Only 4 values are stored.
	tcase_add_test(tc5, testquery13);	// should be valid.	

	printf("testquery16 uses negative 'want #'. must return null to keys.\n");
//#########
//######### IMPORTANT!#########: Check testquery16 with Tanzim's code 
//#########			 if it does what it's supposed to do!
//#########
	//check for want match < 0.
	// Refer to testquery3
	//	table 1 and 1 query call using 			"kilometres <40.0".
	//	Want *-2* of 2 matches stored in keys_found. 
	tcase_add_test(tc5, testquery16);	//should FAIL.

	suite_add_tcase(s3, tc5);

	sr = srunner_create(s3);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);
	printf("\n");

//##############################################################################
/*######  tests 17-18,23-24,31. INVALID tests: Non-existing conditions.   #######*/

	printf("\n");
	Suite *s4 = suite_create("Suite 4 - tests 17-18,23-24,31.INVALID tests:Non-existing conditions");
	TCase *tc6 = tcase_create("testing with non-existing conditions.");
	tcase_set_timeout(tc6, TESTTIMEOUT);
	
	//non-existing column (1 predicate)
	//table 1				"lowTemperature <-242.9"
	tcase_add_test(tc6, testquery17);

	//1 existing column (2 predicates)
	//table 1				"lowTemperature <-242.9, stops > 3"
	tcase_add_test(tc6, testquery18);

	//non-existing columns. Check with 2 tables.
	//store values only into tables 1 and 2.
	//valid predicate for table 1 with 1 match.		"stops > 10"
	//invalid predicate for table 2 that exists in table 1.	"name = Kipling"
	//invalid predicate for table 4 that exists in table 1.	"name = Kipling"
	tcase_add_test(tc6, testquery23);

	//Check for blank predicate with table 1	""	(refer to testquery17 )
	tcase_add_test(tc6, testquery24);

	//Check for missing value		"stops < "
	//Code similar to testquery24
	tcase_add_test(tc6, testquery31);

	suite_add_tcase(s4, tc6);
	sr = srunner_create(s4);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

//##############################################################################
//######  	tests 25-28,30. INVALID tests: data types.  	      ##########
	printf("\n");
	Suite *s5 = suite_create("Suite 5 - tests 25-28,30. INVALID tests: data types");
	TCase *tc7 = tcase_create("testing with invalid data types.");
	tcase_set_timeout(tc7, TESTTIMEOUT);

//from queryconstants.h:
//	4.1.	1 table
//		4.1.1. 1 predicate
//		4.1.2. 3 predicates, 1 valid
//	4.2. 	2 tables
//		4.2.1. 1 table valid
//		4.2.2. all tables invalid
	
	//1 table, 1 predicate		"stops = lalala"	(refer to testquery17 )
	tcase_add_test(tc7, testquery25);

	//1 table, 3 predicates, 1 valid 	"name = Kipling, stops = lalala , kilometres < 20.0"
	//(refer to testquery17 )
	tcase_add_test(tc7, testquery27);

	//2 tables, 1 valid table
	//refer to testquery1 for valid table 1
	//table3:	"brand < -3"	
	tcase_add_test(tc7, testquery28);

	//2 tables, both invalid	(refer to testquery14  and testquery28)
	tcase_add_test(tc7, testquery30);

	suite_add_tcase(s5, tc7);
	sr = srunner_create(s5);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

	return EXIT_SUCCESS;
}
