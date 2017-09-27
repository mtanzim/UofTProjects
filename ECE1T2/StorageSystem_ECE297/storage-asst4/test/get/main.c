/**
 * @file
 * @brief test code for storage_get function.
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
#include <limits.h>

#define SERVEREXEC	"./server"	// Server executable file.
#define SERVEROUT	"server.out"	// File where the server's output is stored.
#define SERVEROUT_MODE	0666		// Permissions of the server ouptut file.
#define FOURTABLES_CONF	"fourtables.conf"// Server configuration file with 4 tables.

#define TESTTIMEOUT	10		// How long to wait for each test to run.

// These settings should correspond to what's in the config file.
#define SERVERHOST	"localhost"	// The hostname where the server is running.
#define SERVERPORT	8888		// The port where the server is running.

#define TABLE1NAME	"subwayLines"		// The first table to use.
#define TABLE2NAME	"cities"		// The second table to use.
#define TABLE3NAME	"cars"
#define TABLE4NAME	"students"
#define TABLENAME_BLANK		""
#define TABLENAME_NOTEXIST	"rubbish"
#define TABLENAME_TOOLONG	"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"

#define KEY		"somekey"	// A key used in the test cases.
#define KEY2		"somekey2"
#define KEY_BLANK	""
#define KEY_NOTEXIST	"rubbish"
#define KEY_TOOLONG	"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"

#define VALUE_NOTEXIST	"name rubbish, stops rubbish"
#define VALUE_INVALID	"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"
#define VALUE_INVALID2	"rubbish"

/*VALID inputs*/
#define VALUEsubway_111_0000	"name Bloor Danforth,stops 31,kilometres 26.2"
#define VALUEsubway_111_1131	"name Bloor Danforth ,stops 31   , kilometres 26.2"

#define VALUEcity_111_0100 	"lowTemperature -7.6, highTemperature +28.0,province Ontario"
#define VALUEcars_111_10	"brand BMW ,price 43210.98"
#define VALUEstudents_111_00	"id 991234567,Grade 87.6"

#define VALUEsubway_111_0001	"name Bloor Danforth,stops 31, kilometres 26.2"
#define VALUEcars_111_00	"brand BMW,price 43210.98"
#define VALUEcity_111_0000 	"lowTemperature -7.6,highTemperature +28.0,province Ontario"

/*INVALID inputs*/
/*Commas and spaces*/
#define INVALUEsubway_000_0000	"nameBloorDanforthstops31kilometres26.2"
#define INVALUEsubway_113_2_121	"name Bloor Danforth stops 31  , kilometres 26.2"
#define INVALUEcars_112_11	"brand BMW  price 43210.98"
#define INVALUEstudents_112_00	"id 991234567Grade 87.6"

/*Spelling table wrong*/
#define IN2111_TABLE1NAME	"subwayLine"
#define IN2112_TABLE1NAME	"ubwayLines"
#define IN2113_TABLE1NAMEd	"dubwayLines"
#define IN2117_TABLE2NAME	"ciTies"
#define IN2112_TABLE3NAME	"carss"
#define IN2116_TABLE4NAME	"studentS"

#define IN2114_TABLE2NAME	"Cities"
#define IN2116_TABLE3NAME	"Car"
#define IN2113_TABLE4NAME	"sstudents"

/*Spelling column wrong*/
#define IN2124_T1C1		"naMe Bloor Danforth,stops 31,kilometres 26.2"
#define IN2124_T2C3		"lowTemperature -7.6,highTemperature +28.0,provinces Ontario"
#define IN2121_T3C2		"brand BMW,pric 43210.98"
#define IN2122_T4C2		"id 991234567 ,Grades 87.6"

/*Missing column params */
#define INVALUE_32_tb4_t1c1	"stops 31,kilometres 26.2"
#define INVALUE_32_tb4_t1c1b 	"name,stops 31, kilometres 26.2"
#define INVALUE_32_tb4_t1c2	"name Bloor Danforth,kilometres 26.2"

/*Invalid input data types*/
#define INVALUE_411	"name 0.3 ,stops 31,kilometres 26.2"
#define INVALUE_412	"name 99999999999 ,stops 31,kilometres 26.2"
#define INVALUE_413	"name -99999999999 ,stops 31,kilometres 26.2"

#define INVALUE_414	"name Bloor Danforth,stops lalala ,kilometres 26.2"
#define INVALUE_415	"name Bloor Danforth,stops 5.99 ,kilometres 26.2"
#define INVALUE_416	"name Bloor Danforth,stops -99 ,kilometres 26.2"

#define INVALUE_417	"name Bloor Danforth,stops 31,kilometres -26.2"
#define INVALUE_418	"name Bloor Danforth,stops 31,kilometres 26"
#define INVALUE_419	"name Bloor Danforth,stops 31,kilometres -26"

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

START_TEST (testVALIDINPUTS1table)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEsubway_111_0000, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0000) != 0, "Value on disk does not match VALID input.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_1131, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_1131) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testVALIDINPUTS4tables)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEsubway_111_0000, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0000) != 0, "Value on disk does not match VALID input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0100, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for CITIES table.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0100) != 0, "Got wrong value for CITIES table.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_10, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for CARS table.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_10) != 0, "Got wrong value for CARS table.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for STUDENTS table.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for STUDENTS table.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_1131, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_1131) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testIN2111oTABLE1NAME)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEsubway_111_0000, sizeof r.value);
	int status = storage_set(IN2111_TABLE1NAME, KEY, &r, conn);
	fail_if(status != -1, "Should fail setting INVALID table name.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(IN2111_TABLE1NAME, KEY, &r, conn);
	fail_if(status != -1, "Should have error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0000) == 0, "Should not store using INVALID table name.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");	

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testIN2114oTABLE2NAME)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	int status = storage_set(IN2114_TABLE2NAME, KEY, &r, conn);
	fail_if(status != -1, "Should fail setting INVALID table name.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(IN2114_TABLE2NAME, KEY, &r, conn);
	fail_if(status != -1, "Should have error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) == 0, "Should not store using INVALID table name.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");


	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testIN2116oTABLE3NAME)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	int status = storage_set(IN2116_TABLE3NAME, KEY, &r, conn);
	fail_if(status != -1, "Should fail setting INVALID table name.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(IN2116_TABLE3NAME, KEY, &r, conn);
	fail_if(status != -1, "Should have error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) == 0, "Should not store using INVALID table name.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testIN2113oTABLE4NAME)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	int status = storage_set(IN2113_TABLE4NAME, KEY, &r, conn);
	fail_if(status != -1, "Should fail setting INVALID table name.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(IN2113_TABLE4NAME, KEY, &r, conn);
	fail_if(status != -1, "Should have error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) == 0, "Should not store using INVALID table name.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testIN31tb4ot1)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEsubway_111_0000, sizeof r.value);
	int status = storage_set(TABLENAME_BLANK, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID table name.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_BLANK, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0000) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testIN31tb4ot2)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEsubway_111_0000, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLENAME_BLANK, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_BLANK, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) == 0, "Value on disk should not match input.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testIN31tb4ot12)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEsubway_111_0000, sizeof r.value);
	int status = storage_set(TABLENAME_BLANK, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID table name.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_BLANK, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0000) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLENAME_BLANK, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_BLANK, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) == 0, "Value on disk should not match input.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE32tb4ot1c1)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_32_tb4_t1c1, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_32_tb4_t1c1) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE32tb4ot1c1b)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_32_tb4_t1c1b, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_32_tb4_t1c1b) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE32tb4ot1c2)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_32_tb4_t1c2, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_32_tb4_t1c2) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST


START_TEST (testBLANKKEY)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEsubway_111_0000, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY_BLANK, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID key.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_BLANK, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0000) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name and KEY.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");
	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE411)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_411, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_411) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");
	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE412)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_412, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_412) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE413)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_413, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_413) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE414)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_414, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_414) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");	

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE415)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_415, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_415) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");
	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE416)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_416, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_416) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE417)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_417, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_417) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");	

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE418)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_418, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_418) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUE419)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUE_419, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, INVALUE_419) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");	

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUEoKEYoTOOLONG)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEsubway_111_0000, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY_TOOLONG, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID column names.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_TOOLONG, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0000) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	fail_if(status != 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (testINVALUEoTABLENAMEoTOOLONG)
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, VALUEsubway_111_0000, sizeof r.value);
	int status = storage_set(TABLENAME_TOOLONG, KEY, &r, conn);
	fail_if(status == 0, "Should return error setting INVALID table name.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_TOOLONG, KEY, &r, conn);
	fail_if(status == 0, "Should return error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0000) == 0, "Value on disk should not match input.");

	// Set a key/value in the second table.
	strncpy(r.value, VALUEcity_111_0000, sizeof r.value);
	status = storage_set(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the second table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE2NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcity_111_0000) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the third table.
	strncpy(r.value, VALUEcars_111_00, sizeof r.value);
	status = storage_set(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the third table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE3NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEcars_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value in the fourth table.
	strncpy(r.value, VALUEstudents_111_00, sizeof r.value);
	status = storage_set(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair for VALID table name.");

	// Get a value from the fourth table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE4NAME, KEY, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEstudents_111_00) != 0, "Got wrong value for VALID table and column name.");

	// Set a key/value. using 2nd key to 1st table
	strncpy(r.value, VALUEsubway_111_0001, sizeof r.value);
	status = storage_set(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error setting a key/value pair.");

	// Get a value. using 2nd key to 1st table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY2, &r, conn);
	fail_if(status != 0, "Error getting a value.");
	fail_if(strcmp(r.value, VALUEsubway_111_0001) != 0, "Value on disk does not match VALID input.");

	// Get a value from a non-existent table.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLENAME_NOTEXIST, KEY, &r, conn);
	fail_if(status != -1, "Getting from a non-existing table should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing table didn't set errno correctly.");

	// Get a value from a non-existent key.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY_NOTEXIST, &r, conn);
	fail_if(status != -1, "Getting from a non-existing key should return an error.");
	fail_if(errno != ERR_TABLE_NOT_FOUND, "Getting from a non-existing key didn't set errno correctly.");

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
	
/*########################     1     ########################*/
	printf("\n");
	Suite *s = suite_create("set1: 1: VALID inputs. All must produce VALID returns");
	TCase *tc1 = tcase_create("testcase set 1.");
	tcase_set_timeout(tc1, TESTTIMEOUT);

	tcase_add_test(tc1, testVALIDINPUTS1table);	
	tcase_add_test(tc1, testVALIDINPUTS4tables);

	suite_add_tcase(s, tc1);
	// Run the test suite.
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

/*########################     2.1.1       ########################*/
	printf("\n");
	s = suite_create("set211: 2.1.1.: Table name spelling mistake(s). All must produce INVALID returns");
	TCase *tc211 = tcase_create("testcase set 211");
	tcase_set_timeout(tc211, TESTTIMEOUT);

	tcase_add_test(tc211, testIN2111oTABLE1NAME);
	tcase_add_test(tc211, testIN2114oTABLE2NAME);
	tcase_add_test(tc211, testIN2116oTABLE3NAME);
	tcase_add_test(tc211, testIN2113oTABLE4NAME);

	suite_add_tcase(s, tc211);
	// Run the test suite.
	sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

/*########################     3.1      ########################*/
	printf("\n");
	s = suite_create("set31: 3.1.: Missing table parameter(s). All must produce INVALID returns");
	TCase *tc31 = tcase_create("testcase set 3.1");
	tcase_set_timeout(tc31, TESTTIMEOUT);
	
	/*Missing table name: use TABLENAME_BLANK*/
	tcase_add_test(tc31, testIN31tb4ot1);	/* TABLENAME_BLANK for table 1*/
	tcase_add_test(tc31, testIN31tb4ot2);	/* TABLENAME_BLANK for table 2*/
	tcase_add_test(tc31, testIN31tb4ot12);	/* TABLENAME_BLANK for table 1 and table 2*/

	suite_add_tcase(s, tc31);
	// Run the test suite.
	sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

/*########################     3.2      ########################*/
	printf("\n");
	s = suite_create("set32: 3.2.: Missing column parameter. All must produce INVALID returns");
	TCase *tc32 = tcase_create("testcase set 3.2");
	tcase_set_timeout(tc32, TESTTIMEOUT);
	
	/*Missing column name:*/
	tcase_add_test(tc32, testINVALUE32tb4ot1c1);	/* blank 1st column for table 1*/
	tcase_add_test(tc32, testINVALUE32tb4ot1c1b);
	tcase_add_test(tc32, testINVALUE32tb4ot1c2);	/* blank 2nd column for table 1*/

	suite_add_tcase(s, tc32);
	// Run the test suite.
	sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

/*########################     3.K      ########################*/
	printf("\n");
	s = suite_create("set3K: 3.K.: blank key. All must produce INVALID returns");
	TCase *tc3K = tcase_create("testcase set 3.K");
	tcase_set_timeout(tc3K, TESTTIMEOUT);
	
	tcase_add_test(tc3K, testBLANKKEY);	/* blank key*/

	suite_add_tcase(s, tc3K);
	// Run the test suite.
	sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

/*########################     4.1.     ########################*/
	printf("\n");
	s = suite_create("set41: 4.1.: invalid data type. All must produce INVALID returns");
	TCase *tc41 = tcase_create("testcase set 4.1.");
	tcase_set_timeout(tc41, TESTTIMEOUT);
	
	tcase_add_test(tc41, testINVALUE411);
	tcase_add_test(tc41, testINVALUE412);
	tcase_add_test(tc41, testINVALUE413);

	tcase_add_test(tc41, testINVALUE414);
	tcase_add_test(tc41, testINVALUE415);
	tcase_add_test(tc41, testINVALUE416);

	tcase_add_test(tc41, testINVALUE417);
	tcase_add_test(tc41, testINVALUE418);
	tcase_add_test(tc41, testINVALUE419);

	suite_add_tcase(s, tc41);
	// Run the test suite.
	sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

/*########################     5     ########################*/
	printf("\n");
	s = suite_create("set5: 5.: Length too long. All must produce INVALID returns");
	TCase *tc5 = tcase_create("testcase set 5.");
	tcase_set_timeout(tc5, TESTTIMEOUT);
	
	tcase_add_test(tc5, testINVALUEoKEYoTOOLONG);
	tcase_add_test(tc5, testINVALUEoTABLENAMEoTOOLONG);

	suite_add_tcase(s, tc5);
	// Run the test suite.
	sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

	return EXIT_SUCCESS;
}

