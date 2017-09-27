/**
 * @file
 * @brief This file contains the implementation of the storage server
 * interface as specified in storage.h.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "storage.h"
#include "utils.h"
#include "errno.h"
#include <time.h>

//#define MAX_CMD_LEN 1000;
//int MAX_CMD_LEN = 1000;

//end to end execution timing parameters
//struct timeval start_time, end_time;

//time_t start, end;




/**
 * @brief This is just a minimal stub implementation.  You should modify it 
 * according to your design.
 * @param hostname The hostname specified in the client configuration file
 * @param port An integer specifying the port of the client
 * 
 * @return returns a pointer to socket    
 */


void* storage_connect(const char *hostname, const int port)
{
	// checks whether it recieved valid parameters
	if (hostname == NULL || port <= 0)
	{
		printf ("Invalid Parameters");
		errno = ERR_INVALID_PARAM;
		return NULL;
	}

	// Create a socket.
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return NULL;

	// Get info about the server.
	struct addrinfo serveraddr, *res;
	memset(&serveraddr, 0, sizeof serveraddr);
	serveraddr.ai_family = AF_UNSPEC;
	serveraddr.ai_socktype = SOCK_STREAM;
	char portstr[MAX_PORT_LEN];
	snprintf(portstr, sizeof portstr, "%d", port);
	int status = getaddrinfo(hostname, portstr, &serveraddr, &res);
	if (status != 0)
		return NULL;

	// Connect to the server.
	status = connect(sock, res->ai_addr, res->ai_addrlen);
	if (status != 0)
		return NULL;

	return (void*) sock;
}


/**
 * @brief This function processes FIND commands from the user
 *
 * @param table c-string containing the name of the file to be opened
 * @param predicates c-string containing search fields
 * @param keys 2-D arrays to hold found records
 * @param max_keys constant to indicate how many keys are required
 * @conn  pointer to the socket, to maintain connection
 *
 * @return 0 if successful, -1 otherwise
 * 
 * The function takes client arguments, and checks whether they are valid or not.
 * Then it communicates with the server to retreive the key within the specified table.
 * The user is notified if an error has occured, and also if the function was successful.
 */

int storage_query(const char *table, const char *predicates, char **keys, 
		const int max_keys, void *conn)
{


	//for temporary testing, stop using once predicates are reformatted
	//strcpy (predicates, "price < 53210.98, name = Tanzim");


/*

	printf ("Running query.\n");
	printf("The input:\n");
	printf("table: %s\n", table);
	printf("predicates: %s\n", predicates);
	printf("1st key to store 1st query result %d: %s %s\n", 0, keys[0]);
	printf("max keys: %d\n\n\n", max_keys);
*/




	int sock = (int) conn;

	if (table == NULL || predicates == NULL || keys == NULL || conn == NULL)
	{
		printf ("Invalid Parameters");
		errno = ERR_INVALID_PARAM;
		return -1;
	}

	else
	{



		
		struct ind_predicates{
			char whole_pred[MAX_VALUE_LEN];
			char arg_a[MAX_VALUE_LEN];
			char operator [MAX_VALUE_LEN];
			char arg_b[MAX_VALUE_LEN];
		}; 

		struct ind_predicates pred_list[100];

		char *p;
		char temp_pred[MAX_VALUE_LEN];
		//count the number of predicates by comma
		int pred_count = 1;

		strcpy (temp_pred,predicates);
		//printf("temporary predicate:%s\n", temp_pred);
		p = strtok(temp_pred, ",");
		//printf("predicate 0:%s\n", p);
		strcpy (pred_list[0].whole_pred,p);

		
		//split up the predicates
		do{
    			p = strtok(NULL, ",");
    			if(p)
			{	 
				//store all matching keys in the keys c-string array
				//printf("predicate %d: %s\n",pred_count, p);
				strcpy (pred_list[pred_count].whole_pred,p);
				pred_count ++;
			}

  		} while(p);


		int sc = 0;

		while (sc<pred_count)
		{
			//printf("predicates before in struct %d: %s\n",sc, pred_list[sc].whole_pred);
			sc++;
		}


		
		//get rid of white spaces in the beginning

		int a_sec = 0;
		char *bph;

		while (a_sec<pred_count)
		{

			

			bph = pred_list[a_sec].whole_pred;
			int total_length = strlen(bph);
			//algorithm to eliminate unncessary whitespaces in the beginning of the string
			//int index =0;
			int space_count = 0;

			
			
			while (bph[space_count] == 32)
			{
				space_count++;
			}
				//printf ("The number of spaces in the beginning %d\n", space_count);
			if (space_count != 0)
			{
				int s_index =space_count;
				while (s_index<total_length || bph[s_index] != 0)
				{
					bph[s_index-space_count]=bph[s_index];
					bph[s_index] = 32;
					s_index++;
				}
			}

			//printf ("after beginning space elim:%s\n", bph);

			strcpy (pred_list[a_sec].whole_pred,bph);
			a_sec++;

		}




		//eliminate extra white spaces in the end of predicates

		//printf ("before modification: %s\n", all_col);

		int sec = 0;
		char *iph;

		while (sec<pred_count)
		{

			iph = pred_list[sec].whole_pred;
			//algorithm to eliminate unncessary whitespaces in the end of the string
			int space_count = strlen(iph);
			do{

				if (iph[space_count-1] == 32)
				{
					iph[space_count-1] = 0;
				}

				space_count--;
			}while (iph[space_count-1] == 32);

			strcpy (pred_list[sec].whole_pred,iph);
			sec++;
		}
	
		//printf ("after modification: %s\n", all_col);




		int sc2 = 0;
		while (sc2 < pred_count)
		{
			//printf("predicates after in struct %d: %s\n",sc2,pred_list[sc2].whole_pred);
			sc2++;
		}

		char final_predicate[MAX_VALUE_LEN];

		strcpy (final_predicate,"");

		int sc3 = 0;
		while (sc3 < pred_count)
		{
			strcat (final_predicate,pred_list[sc3].whole_pred);
			strcat (final_predicate,",");
			//printf("predicates after in struct %d: %s\n",sc2,pred_list[sc2].whole_pred);
			sc3++;
		}

		//strcpy (predicates,final_predicate);

		//printf("predicates after end AND beginning space elimination: %s\n", final_predicate);

		//spaces have have been eliminated, now add spaces before and after operators

		int pred_count_b = 1;
//		char after_arg_a[MAX_VALUE_LEN];
		char *tph;

		


		tph = strtok(final_predicate, "<=>");
		//printf ("%s\n",tph);
		strcpy (pred_list[0].arg_a,tph);

		tph = strtok (NULL, ",");
		//printf ("%s\n",tph);
		strcpy (pred_list[0].arg_b,tph);

	
		do 
		{
			if (tph && pred_count>1)
			{

				tph = strtok(NULL, "<=>");
				//printf ("%s\n",tph);
				strcpy (pred_list[pred_count_b].arg_a,tph);

				tph = strtok (NULL, ",");
				//printf ("%s\n",tph);
				strcpy (pred_list[pred_count_b].arg_b,tph);
			}
			pred_count_b++;
		}while (pred_count_b < pred_count && tph != NULL);

		//now find the operators

		//int which_op = -1;

		int op_count = 0;
		
		while (op_count<pred_count)
		{
			int op_count_in = 0;
			while (op_count_in < strlen (pred_list[op_count].whole_pred))
			{
				if (pred_list[op_count].whole_pred[op_count_in] == 60)
				{
					strcpy(pred_list[op_count].operator, "<");
					break;
				}
				else if (pred_list[op_count].whole_pred[op_count_in] == 61)
				{
					strcpy(pred_list[op_count].operator, "=");
					break;
				}
				else if (pred_list[op_count].whole_pred[op_count_in] == 62)
				{
					strcpy(pred_list[op_count].operator, ">");
					break;
				}
/*
				else
				{
					printf ("Invalid Parameters.\n");
					errno = ERR_INVALID_PARAM;						
					return -1;
				}
*/

				op_count_in ++;
			}
			op_count++;
		}


//////////////////////////////////////////////////////////eliminate while space again

		int a_sec_arga = 0;
		char *bph_arga;

		while (a_sec_arga<pred_count)
		{

			

			bph_arga = pred_list[a_sec_arga].arg_a;
			int total_length = strlen(bph_arga);
			//algorithm to eliminate unncessary whitespaces in the beginning of the string
			//int index =0;
			int space_count = 0;

			
			
			while (bph_arga[space_count] == 32)
			{
				space_count++;
			}
				//printf ("The number of spaces in the beginning %d\n", space_count);
			if (space_count != 0)
			{
				int s_index =space_count;
				while (s_index<total_length || bph_arga[s_index] != 0)
				{
					bph_arga[s_index-space_count]=bph_arga[s_index];
					bph_arga[s_index] = 32;
					s_index++;
				}
			}

			//printf ("after beginning space elim:%s\n", bph_arga);

			strcpy (pred_list[a_sec_arga].arg_a,bph_arga);
			a_sec_arga++;

		}




		//eliminate extra white spaces in the end of predicates

		//printf ("before modification: %s\n", all_col);

		int sec_arga = 0;
		char *iph_arga;

		while (sec_arga<pred_count)
		{

			iph_arga = pred_list[sec_arga].arg_a;
			//algorithm to eliminate unncessary whitespaces in the end of the string
			int space_count = strlen(iph_arga);
			do{

				if (iph_arga[space_count-1] == 32)
				{
					iph_arga[space_count-1] = 0;
				}

				space_count--;
			}while (iph_arga[space_count-1] == 32);

			strcpy (pred_list[sec_arga].arg_a,iph_arga);
			sec_arga++;
		}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		int a_sec_argb = 0;
		char *bph_argb;

		while (a_sec_argb<pred_count)
		{

			

			bph_argb = pred_list[a_sec_argb].arg_b;
			int total_length = strlen(bph_argb);
			//algorithm to eliminate unncessary whitespaces in the beginning of the string
			//int index =0;
			int space_count = 0;

			
			
			while (bph_argb[space_count] == 32)
			{
				space_count++;
			}
				//printf ("The number of spaces in the beginning %d\n", space_count);
			if (space_count != 0)
			{
				int s_index =space_count;
				while (s_index<total_length || bph_argb[s_index] != 0)
				{
					bph_argb[s_index-space_count]=bph_argb[s_index];
					bph_argb[s_index] = 32;
					s_index++;
				}
			}

			//printf ("after beginning space elim:%s\n", bph_argb);

			strcpy (pred_list[a_sec_argb].arg_b,bph_argb);
			a_sec_argb++;

		}




		//eliminate extra white spaces in the end of predicates

		//printf ("before modification: %s\n", all_col);

		int sec_argb = 0;
		char *iph_argb;

		while (sec_argb<pred_count)
		{

			iph_argb = pred_list[sec_argb].arg_b;
			//algorithm to eliminate unncessary whitespaces in the end of the string
			int space_count = strlen(iph_argb);
			do{

				if (iph_argb[space_count-1] == 32)
				{
					iph_argb[space_count-1] = 0;
				}

				space_count--;
			}while (iph_argb[space_count-1] == 32);

			strcpy (pred_list[sec_argb].arg_b,iph_argb);
			sec_argb++;
		}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		
		//checking parts of predicates
		int sc4 = 0;
		while (sc4 < pred_count)
		{
			//printf("first argument in pred %d is: :%s: \n",sc4,pred_list[sc4].arg_a);
			//printf("operator in pred %d is::%s:\n",sc4,pred_list[sc4].operator);
			//printf("second argument in pred1 %d is:  :%s:  \n",sc4,pred_list[sc4].arg_b);
			sc4++;
		}


		//printf( "asdasd\n");
		int final_counter=0;
	
		//char pred_to_ser[MAX_VALUE_LEN] = "";
		char new_predicates[MAX_VALUE_LEN];

		strcpy(new_predicates,"");

		
	
		while (final_counter < pred_count)
		{

			strcat(new_predicates, pred_list[final_counter].arg_a);
			strcat(new_predicates, " ");
			strcat (new_predicates, pred_list[final_counter].operator);
			strcat(new_predicates, " ");
			strcat(new_predicates, pred_list[final_counter].arg_b);
			strcat(new_predicates, ",");
			
			final_counter ++;
		}

	




		char buf[MAX_CMD_LEN];
		memset(buf, 0, sizeof buf);

		//char max_keys_str = something(max_keys);

		//send parsed predicates to server
		snprintf(buf, sizeof buf, "FIND %s %d %s\n",table, max_keys , new_predicates);
		printf ("Buffer to server:%s", buf);

		//process server's response
		if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0)
		//if (sendall(sock, buf, strlen(buf)) == 0) 
		{

			printf ("Buffer from server:%s\n", buf);
			char *all_col;
			char type [MAX_VALUE_LEN];
			char value [MAX_VALUE_LEN];

			char k_f_s [MAX_VALUE_LEN];


			all_col = strtok(buf, " ");

			strcpy (type, all_col);

			//printf ("Type:%s\n", type);

			

			// checks for possible errors, and sets errno accordingly
			if(strcmp(type, "ERROR") == 0)
			{

				all_col = strtok(NULL , "\n");
				strcpy (value, all_col);
				//printf ("Value:%s\n",value );

				if(strcmp(value, "TableNotFound") == 0)
				{
					printf("Error: Table not Found!\n");
					errno = ERR_TABLE_NOT_FOUND;
					return -1;
				}
				else if (strcmp(value, "ColumnNotFound") == 0)
				{
					printf("Error: Column not Found!\n");
					errno = ERR_KEY_NOT_FOUND;
					return -1;
				}
				else
				{

					printf ("Invalid Parameters.\n");
					errno = ERR_INVALID_PARAM;						
					return -1;
				}
					
			}
			// if no errors have been found so far, set record->value
			else if (strcmp(type, "Found") == 0)
			{
				//printf ("Type:%s\n", type);
				all_col = strtok(NULL, ":");

				strcpy (k_f_s, all_col);
				
				printf("Keys found:%s\n",k_f_s);

				int key_count = 0;

				
  					do {

						if (key_count > max_keys){break;}
    						all_col = strtok(NULL, ",");
    						if(all_col)
						{	 
							//store all matching keys in the keys c-string array
							strcpy (keys[key_count],all_col);
							printf ("Keys are:%s\n", keys[key_count]);
							key_count ++;
						}
				

  					} while(all_col);
					//printf("returning %d\n",atoi(k_f_s));
					return atoi (k_f_s);
				
			}
			// if no errors have been found, and nothing has been set, it's an unknown error, set
                        // errno accordingly
			else
			{
				errno = ERR_UNKNOWN;
				printf ("An unknown error has occured.\n");
				return -1;
			}		
		}
		// communication with the server wasn't successful due to invalid parameters
		else
		{
			
			printf ("Invalid Parameters due to miscommunications.\n");
			errno = ERR_INVALID_PARAM;						
			return -1;	
		}
	}

	return -1;
}

		


/**
 * @brief This function processes GET commands from the user
 *
 * @param table c-string containing the name of the file to be opened
 * @param key c-string containing the key to be searched
 * @param record pointer to the structure containing the required value
 * @conn  pointer to the socket, to maintain connection
 *
 * @return 0 if successful, -1 otherwise
 *
 * The function takes client arguments, and communicates with the server to either retreive 
 * specified value into the specified table.  The user is notified on the
 * status of the server communication.
 */

int storage_get(const char *table, const char *key, struct storage_record *record, void *conn)
{

	//time(&start);

	printf ("Running storage_get.\n");

	// Connection is really just a socket file descriptor.
	int sock = (int)conn;

	// checks the validity of given inputs
	if (table == NULL || key == NULL || record == NULL || conn == NULL)
	{
		printf ("Invalid Parameters");
		errno = ERR_INVALID_PARAM;
		return -1;
	}
	else
	{
		

		//  Send some data.
		char buf[MAX_CMD_LEN];
		memset(buf, 0, sizeof buf);

		//send table and key to the server
		snprintf(buf, sizeof buf, "GET %s %s\n", table, key);

		printf ("storage_get:Buffer to server: %s\n", buf);
		
		// Ensures the functionality of sendall and recvline
		if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) 
		{

			printf ("Buffer from server: %s\n", buf);



			char buf_f_c [MAX_CMD_LEN];
			char version [MAX_CMD_LEN];

			strcpy (buf_f_c,buf);

/*

			strtok (buf_f_c, "&");
			strcpy (version, strtok(NULL,"&"));
		
			printf ("Current Version: %s\n", version);

			//setting the metadata
			record -> metadata[0] = atoi(version);
*/


			//get timing parameters
			char *time_col;
			char seconds [10];
			char microsec [30];

			time_col = strtok(buf, "~");
			time_col = strtok(NULL, ".");

			strcpy (seconds, time_col);

			time_col = strtok(NULL, " ~");
			strcpy (microsec, time_col);

			printf ("Seconds taken for get:%s\n", seconds);
			printf ("Microseconds taken for get:%s\n", microsec);
			//done getting time


			char type [MAX_VALUE_LEN];
			char value [MAX_VALUE_LEN];

		
			//temporary pointer to use with strtok()
			char *all_col;

			strcpy (type, strtok(buf, " "));

			
			// checks for possible errors, and sets errno accordingly
			if(strcmp(type, "ERROR") == 0)
			{

				strcpy (value, strtok(NULL, " "));

				if(strcmp(value, "TableNotFound") == 0)
				{
					printf("Error: Table not Found!\n");
					errno = ERR_TABLE_NOT_FOUND;
					return -1;
				}
				else if (strcmp(value, "KeyNotFound") == 0)
				{
					printf("Error: Key Not Found!\n");
					errno = ERR_KEY_NOT_FOUND;
					return -1;
				}
			}
			// if no errors have been found so far, set record->value
			else if (strcmp(type, "OK") == 0)
			{



//tranasction protocol
/*
sscanf(cmd, "SET %s %s %s %s %s", table, key, temp_version, tempNewValue);

so for set commands send over:

SET tableName key version(metadata) newRecordValue

get i'm returning to you the version between &s after the server processing time stuff

i.e. ~time~ &5&
for version 5
*/


				strcpy (record->value, "");
				//strcpy (record->value, strtok	
				//printf ("column is %s\n", buf);
				//printf("Keys found: \n");
				//int key_count = 0;
				//all_col = strtok(buf, " ");
  				do {
    					all_col = strtok(NULL, " :");
					//printf ("column is %s\n", all_col);
    					if(all_col)
					{	 
						//store all matching keys in the keys c-string array
						strcat (record->value,all_col);
						strcat (record->value, " ");
					}

  				} while(all_col);
			
				printf ("Record successfully retreived.\n");		
				printf ("The record holds the following:%s\n", record->value);

				strtok (buf_f_c, "&");
				strcpy (version, strtok(NULL,"&"));
		
				printf ("Current Version: %s\n", version);

				//setting the metadata
				record -> metadata[0] = atoi(version);


				return 0;
				
			}
			// if no errors have been found, and nothing has been set, it's an unknown error, set
                        // errno accordingly
			else
			{
				errno = ERR_UNKNOWN;
				printf ("An unknown error has occured.\n");
				return -1;
			}		
		}
		// communication with the server wasn't successful due to invalid parameters
		else
		{
			
			printf ("Invalid Parameters.");
			errno = ERR_INVALID_PARAM;						
			return -1;	
		}
	}
	
	
	
	return -1;
}


/**
 * @brief This function processes SET commands from the user
 *
 * @param table c-string containing the name of the file to be opened
 * @param key c-string containing the key to be searched
 * @param record pointer to the structure containing the required value
 * @conn  pointer to the socket, to maintain connection
 *
 * @return 0 if successful, -1 otherwise
 *
 * The function takes client arguments, and communicates with the server to either set 
 * or delete the specified value into the specified table.  The user is notified on the
 * status of the server communication.
 */
int storage_set(const char *table, const char *key, struct storage_record *record, void *conn)
{

	const int ETA = -7;


	//time(&start);
	//printf ("storage_set starting at: %d\n", start_time);
	
	

	printf ("Running storage_set.\n");
	// Connection is really just a socket file descriptor.
	int sock = (int)conn;

	// checks the validity of given inputs
	if (table == NULL || key == NULL || conn == NULL)
	{
		printf ("Invalid Parameters!\n");
		errno = ERR_INVALID_PARAM;
		return -1;
	}




//tranasction protocol
/*
sscanf(cmd, "SET %s %s %s %s %s", table, key, temp_version, tempNewValue);

so for set commands send over:

SET tableName key version(metadata) newRecordValue

get i'm returning to you the version between &s after the server processing time stuff

i.e. ~time~ &5&
for version 5
*/


	else
	{
		
		// Send some data.
		char buf[MAX_CMD_LEN];
		memset(buf, 0, sizeof buf);


		//process to handle deletion of a record from files
		if (record == NULL)
		{
			//printf ("We have to delete.\n");

			//strcpy (record->value, "DELETE");
			//record -> metadata [0] = 0;

			snprintf(buf, sizeof buf, "SET %s %s %s %s\n", table, key, "0", "DELETE");

			printf ("Buffer to server:\n%s\n", buf);

			if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) 
			{


				printf ("Buffer from server:\n%s\n", buf);


				char type [MAX_VALUE_LEN];
				char value [MAX_VALUE_LEN];
				//char to_display [MAX_VALUE_LEN];

				//receiving what the server sent


/*
//////////////////////////
				//printf ("asdasdasdqsqdqsd2wdqwda\n");


				//get timing parameters
				char *time2_col;
				char seconds2 [10];
				char microsec2 [30];

				time2_col = strtok(buf, "~");
				time2_col = strtok(NULL, ".");

				strcpy (seconds2, time2_col);

				time2_col = strtok(NULL, " ~");
				strcpy (microsec2, time2_col);

				printf ("Seconds taken for set:%s\n", seconds2);
				printf ("Microseconds taken for set:%s\n", microsec2);
				//done getting time


////////////////////////////
*/





				strcpy(type, strtok(buf, " "));


				// checks for possible errors, and sets errno accordingly
				if(strcmp(type, "ERROR") == 0)
				{
					strcpy(value, strtok(NULL, " "));

					if(strcmp(value, "TableNotFound") == 0)
					{
						printf("Error: Table not Found!\n");
						errno = ERR_TABLE_NOT_FOUND;
						return -1;				
					}
					else if (strcmp(value, "KeyNotFound") == 0)
					{
						printf("Error: Key Not Found!\n");
						errno = ERR_KEY_NOT_FOUND;
						return -1;				
					}
					else if (strcmp(value, "InvalidParameter") == 0)
					{
						printf("Error: Invalid Parameters!\n");
						errno = ERR_INVALID_PARAM;
						return -1;				
					}
					else if (strcmp(value, "TransactionAbort") == 0)
					{
						printf("Error: Transaction Aborted!\n");
						errno = ERR_INVALID_PARAM;
						return ETA;				
					}

				}
				// if no errors have been found so far, set record->value
				else if (strcmp(type, "OK") == 0)
				{


					//printf ("it worked\n");
			

					strcpy(value, strtok(NULL, " "));


					//printf ("The set value is: %s\n", value);


					// new record has been inserted through the server, notify user
					if (strcmp(value,"New")==0)
					{







						//printf ("It comes here\n");
						printf ("Table updated with the new value.\n");
					}
					// record has been deleted through the server, notify user
					else if (strcmp(value,"Deleted")==0)
					{


						printf ("Value deleted from the table.\n");
					}

					return 0;
				}
				// if it gets to here without returning, and unknown error has occured
				else
				{
					errno = ERR_UNKNOWN;
					printf ("An unknown error has occured.\n");
					return -1;					
				}		
			}

		}

		


		//the following code parses the record value by eliminating optional whitespaces, so the server can
		//process it

		char *all_col;
		//temporary pointer to use with strtok() while parsing server response
		//char *temp_server_ptr;

		char temp_rec_value [MAX_VALUE_LEN];

		strcpy (temp_rec_value, record->value);
		strcpy (record->value,"");


		//structure to hold column information
		struct value_holder{
			char col_name[MAX_VALUE_LEN];
			char col_value[MAX_VALUE_LEN];
		};
		
		struct value_holder col_holder [10];
		int col_count = 0;

		all_col = strtok(temp_rec_value, " ");
		

		//split up the record->value into columns and reformat so the server can process it
  		do {

			
			strcpy (col_holder[col_count].col_name, all_col);

    			all_col = strtok(NULL, ",");

			if (all_col)
			{
				//printf ("current column value is:%s\n", all_col);
				//printf ("strlen %d\n",space_count);
				//printf ("the current char is: %d\n", all_col[space_count-1]);
				//printf ("before modification: %s\n", all_col);

				//algorithm to eliminate unncessary whitespaces
				int space_count = strlen(all_col);
				do{

					//printf ("strlen %d\n",space_count);
					//printf ("the current char is: %d\n", all_col[space_count-1]);

					if (all_col[space_count-1] == 32)
					{
						all_col[space_count-1] = 0;
					}

					space_count--;
				}while (all_col[space_count-1] == 32);
	
				//printf ("after modification: %s\n", all_col);

				strcpy (col_holder[col_count].col_value, all_col);
				all_col = strtok(NULL, " ");
			}
			col_count++;


			
  		} while(all_col);

		

		int z = 0;
		
		//reformat the record-> so the server will be able to process it
		while (z<col_count)
		{
			strcat (record->value, col_holder[z].col_name);
			strcat (record->value, " ");
			strcat (record->value, col_holder[z].col_value);
			strcat (record->value, ",");
			z++;
		}
	




		printf ("The server gets:\n");
		printf ("Table Name:%s\n", table);
		printf ("Key:%s\n", key);
		printf ("Value:%s\n", record->value);
		printf ("End of debug.\n\n");


		
		// record value now holds all columns information, seperated by commas, send it to the server

		snprintf(buf, sizeof buf, "SET %s %s %d %s\n", table, key, record->metadata[0], record->value);
		printf ("Buffer to server:\n%s\n", buf);

		if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) 
		{

			printf ("Buffer from server:\n%s\n", buf);

			//char buf_for_time[1000];
			
			






/*////////////problem!!!!!!!!!!!/

					strcpy(buf_for_time, buf);

					//printf ("asdasdasdqsqdqsd2wdqwda\n");


					//get timing parameters
					char *time3_col;
					char seconds3 [100];
					char microsec3 [600];

					printf ("wth?");

					time3_col = strtok(buf_for_time, "~");
					
					time3_col = strtok(NULL, ".");

					strcpy (seconds3, time3_col);

					time3_col = strtok(NULL, " ~");
					strcpy (microsec3, time3_col);

					printf ("Seconds taken for set:%s\n", seconds3);
					printf ("Microseconds taken for set:%s\n", microsec3);
					//done getting time




//////////////////!!!!!!!!!!!!!!!!!!!!!!!*/




			char type [MAX_VALUE_LEN];
			char value [MAX_VALUE_LEN];
			//char to_display [MAX_VALUE_LEN];

			//receiving what the server sent

			strcpy(type, strtok(buf, " "));



			// checks for possible errors, and sets errno accordingly
			if(strcmp(type, "ERROR") == 0)
			{
				strcpy(value, strtok(NULL, " "));

				if(strcmp(value, "TableNotFound") == 0)
				{
					printf("Error: Table not Found!\n");
					errno = ERR_TABLE_NOT_FOUND;
					return -1;				
				}
				else if (strcmp(value, "KeyNotFound") == 0)
				{
					printf("Error: Key Not Found!\n");
					errno = ERR_KEY_NOT_FOUND;
					return -1;				
				}
				else if (strcmp(value, "InvalidParameter") == 0)
				{
					printf("Error: Invalid Parameters!\n");
					errno = ERR_INVALID_PARAM;
					return -1;				
				}
				else if (strcmp(value, "TransactionAbort") == 0)
				{
					printf("Error: Transaction Aborted!\n");
					errno = ERR_INVALID_PARAM;
					return ETA;
				}
				
			}
			// if no errors have been found so far, set record->value
			else if (strcmp(type, "OK") == 0)
			{
				//printf ("it worked\n");
			

				strcpy(value, strtok(NULL, ":"));




				// new record has been inserted through the server, notify user
				if (strcmp(value,"New")==0)
				{



///////////////////////////////////////////////////////////////////////////////////////////////////


                                      //printf ("asdasdasdqsqdqsd2wdqwda\n");


					//get timing parameters
					char *time3_col;
					char seconds3 [100];
					char microsec3 [600];

					//printf ("wth?");

					time3_col = strtok(NULL, "~");
					
					time3_col = strtok(NULL, ".");

					strcpy (seconds3, time3_col);

					time3_col = strtok(NULL, " ~");
					strcpy (microsec3, time3_col);

					printf ("Seconds taken for set:%s\n", seconds3);
					printf ("Microseconds taken for set:%s\n", microsec3);
					//done getting time

//////////////////////////////////////////////////////////////////////////////////////////////////////





					//printf ("It comes here\n");
					printf ("Table updated with the new value.\n");
				}
				// record has been deleted through the server, notify user
				else if (strcmp(value,"Deleted")==0)
				{



///////////////////////////////////////////////////////////////////////////////////////////////////


                                      //printf ("asdasdasdqsqdqsd2wdqwda\n");


					//get timing parameters
					char *time3_col;
					char seconds3 [100];
					char microsec3 [600];

					//printf ("wth?");

					time3_col = strtok(NULL, "~");
					
					time3_col = strtok(NULL, ".");

					strcpy (seconds3, time3_col);

					time3_col = strtok(NULL, " ~");
					strcpy (microsec3, time3_col);

					printf ("Seconds taken for set:%s\n", seconds3);
					printf ("Microseconds taken for set:%s\n", microsec3);
					//done getting time

//////////////////////////////////////////////////////////////////////////////////////////////////////






					printf ("Value deleted from the table.\n");
				}

				return 0;
			}
			// if it gets to here without returning, and unknown error has occured
			else
			{
				errno = ERR_UNKNOWN;
				printf ("An unknown error has occured.\n");
				return -1;					
			}		
		}
		// communication with the server wasn't successful due to invalid parameters
		else
		{
			
			printf ("Invalid Parameters due to miscommunications.\n");
			errno = ERR_INVALID_PARAM;						
			return -1;							
		}
	}
	
	
	
	return -1;
}



/**
 * @brief Disconnects with the server
 *  
 * @param conn Pointer to the connection
 *
 * @return 0 if successful
 */
int storage_disconnect(void *conn)
{
	if (conn == NULL)
	{
		printf ("Invalid Parameters");
		errno = ERR_INVALID_PARAM;
		return 0;
	}
	// Cleanup
	int sock = (int)conn;
	close(sock);

	return 0;
}

