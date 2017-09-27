/**
 * @file
 * @brief Constants used by the test code for storage_query function.
 *
 * @author Wina Ng
 */

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

#define KEY1	"firstkey"
#define KEY2	"secondkey"
#define KEY3	"thirdkey"
#define KEY4	"fourthkey"

#define TABLE1VAL1	"name Bloor Danforth,stops 31 ,kilometres 26.2"	
#define TABLE1VAL2	"name Kipling, stops 5, kilometres 4.6"
#define TABLE1VAL3	"name some station, stops 16, kilometres 70.0"
#define TABLE1VAL4	"name another station , stops 16,kilometres 50.0"

#define TABLE2VAL1	"lowTemperature -7.6,highTemperature +28.0 ,province province1"
#define TABLE2VAL2	"lowTemperature -242.9,highTemperature -3.5,province province2"
#define TABLE2VAL3	"lowTemperature -242.9, highTemperature +364.1   ,province province3"
#define TABLE2VAL4	"lowTemperature +50.4 , highTemperature +88.0 ,province province4"

#define TABLE3VAL1	"brand BMW ,price 43210.98"
#define TABLE3VAL2	"brand abc , price 210.98"
#define TABLE3VAL3	"brand BMW, price 5000.98"
#define TABLE3VAL4	"brand BMW,price 4000.98"

#define TABLE4VAL1	"id 123456, Grade 87.6"
#define TABLE4VAL2	"id 1, Grade 55.0"
#define TABLE4VAL3	"id 123456, Grade 66.0"
#define TABLE4VAL4	"id 127, Grade 55.0"

/* 
In all the tests you should consider testing corner cases, such as trying to get a non-existent key from a table, doing a query with column names not defined in a table, or storing a record with columns listed in the wrong order. You are free to use any tables you want but it would be a good idea to construct tables with columns of all supported types (strings, integers, floats) 

VALID PREDICATES
FORMAT: 		"ColumnName operator value"

###########
Test cases
1. VALID								#DONE
	1.1. Check with 1 table						#DONE
		1.1.1. 1 predicate					#DONE
		1.1.2. 2 predicates					#DONE
		1.1.3. 3 predicates					#DONE
		1.1.4. 2 predicates not in order			#DONE
	1.2. Check with multiple tables					#DONE
2. Check all applicable <, >, =						#DONE
3. Check spacing and commas						#DONE
	3.1. Valid							#DONE
	3.2. Invalid							#DONE
		3.2.1. no spaces at all, all commas present		#DONE
		3.2.3. No commas, with spaces extra spaces		#DONE
4. Check data types							#DONE
	4.1.	1 table							#DONE
		4.1.1. 1 predicate					#DONE
		4.1.2. 3 predicates, 1 valid				#DONE
	4.2. 	2 tables						#DONE
		4.2.1. 1 table valid					#DONE
		4.2.2. all tables invalid				#DONE
5. check for non-existing conditions (see main.c)			#DONE
6. check for want # of matches:						#DONE
	6.1 more than actual match found				#DONE
	6.2 more than values stored					#DONE
	6.3 less than zero						#DONE
	6.4 Valid							#DONE
	6.5 more than keys allocated					#DONE
7. Check with multiple query calls					#DONE
	7.1 Valid							#DONE
	7.2 Invalid							#DONE
8. Check with multiple predicates					#DONE
9. Check for blank predicate.						#DONE

*/
