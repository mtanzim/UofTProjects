/**
 * @file
 * @brief This file implements various utility functions that are
 * can be used by the storage server and client library. 
 */
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "utils.h"

// keeps track of how many tables are found in the configuration files
int table_count = 0;

/**
 * @brief checks if a file specified in the configuiration file exists,
 * if not, it creates a blank file
 *
 * @param params a pointer to the structure params, so file names and 
 * directory can be accessed
	
 *in. I don't k
 * @return void function doesn't return anything
 *
 * The function takes in configuiration file information. It manipulates the information to 
 * specify the full path where files should be stored. It then creates the directory
 * where files are to be stored, and produces non-existing files within that directory.
 */

void create_new(struct config_params *params)
{

	int i = 0;
	FILE *fp;
	
	// the loop iterates as many times as table_count to create the right number of files
	printf("\n"); 
	while (i<table_count)
	{
		// sets file_path to the full directory so the files can be created in the right place
		char file_path[MAX_PATH_LEN + MAX_TABLE_LEN]= "\0";
		strcat (file_path, params->data_directory);
		strcat (file_path, "/");
		strcat (file_path, tables[i].name);

		
		// makes the directory specified in the configuration file if it doesn't exist
		mkdir(params->data_directory, 0700);
 
		fp=fopen(file_path, "r");

		// creates files according to the configuration file if they don't already exist
		if (fp==NULL)
		{
			printf("%s", tables[i].name);
			printf(" doesn't exist. Creating new.\n");
			///creates a new file with the filename 
			fp=fopen(file_path, "w");
			
		}
		// file already exists
		else
		{
			printf("%s", tables[i].name);
			printf(" already exists.\n");
			fclose(fp);
			
		}
		
		
		i++;
	}
	printf("\n");
} 	


int sendall(const int sock, const char *buf, const size_t len)
{
	size_t tosend = len;
	while (tosend > 0) {
		size_t bytes = send(sock, buf, tosend, 0);
		if (bytes <= 0) 
			break; // send() was not successful, so stop.
		tosend -= bytes;
		buf += bytes;
	};

	return tosend == 0 ? 0 : -1;
}

/**
 * In order to avoid reading more than a line from the stream,
 * this function only reads one byte at a time.  This is very
 * inefficient, and you are free to optimize it or implement your
 * own function.
 */
int recvline(const int sock, char *buf, const size_t buflen)
{
	int status = 0; // Return status.
	size_t bufleft = buflen;

	while (bufleft > 0) {
		// Read one byte from scoket.
		size_t bytes = recv(sock, buf, 1, 0);
		if (bytes <= 0) {
			// recv() was not successful, so stop.
			status = -1;
			break;
		} else if (*buf == '\n') {
			// Found end of line, so stop.
			*buf = 0; // Replace end of line with a null terminator.
			status = 0;
			break;
		} else {
			// Keep going.
			bufleft += 1;
			buf += 1;
		}
	}

	return status;
}


/**in. I don't k
 * @brief Parse and process a line in the config file.
 */
int process_config_line(char *line, struct config_params *params)
{
	// Ignore comments.
	if (line[0] == CONFIG_COMMENT_CHAR)
		return 0;

	// Extract config parameter name and value.
	char name[MAX_CONFIG_LINE_LEN];
	char value[MAX_CONFIG_LINE_LEN];
	int items = sscanf(line, "%s %s\n", name, value);
	char *all_col;

	//Line wasn't as expected.
	if (items != 2)
		return -1;

	// Process this line.
	if (strcmp(name, "server_host") == 0) {
		strncpy(params->server_host, value, sizeof params->server_host);
	} else if (strcmp(name, "server_port") == 0) {
		params->server_port = atoi(value);
	} else if (strcmp(name, "data_directory") == 0) {
		strncpy(params->data_directory, value, sizeof params->data_directory);
	// line reads table, thus the following field is a file name, so it needs to be stored within
	// the table_name[i] structure



	} else if (strcmp(name, "cachepolicy") == 0) {
		strncpy(params->cache_policy, value, sizeof params->cache_policy);
		printf ("Caching is %s.\n",  params->cache_policy);
	
	} else if ((strcmp(name, "cachesize") == 0) && (strcmp(params->cache_policy, "on") == 0)) {
		params->cache_size = atoi(value);
		printf ("Cache size for this session is: %d\n",  params->cache_size);


	} else if (strcmp(name, "concurrency") == 0) {
		params -> concurrency = atoi(value);

		printf ("Concurrency is %d\n", params->concurrency);
		
		if (params->concurrency != 0 || params->concurrency != 1)
		{

			printf ("Concurrency option not properly specified.Exiting.\n");
			return -1;
		}
			
		
	 

	} else if (strcmp(name, "table") == 0) {

		

		struct temp_full_column{
			char name_col[1000];
		};

		struct temp_full_column temp_full_columns[10];
		
		//char type [MAX_TABLE_LEN];

		strncpy (tables[table_count].name, value, strlen(value));
		strcat(tables[table_count].name,".txt");
		


		all_col = strtok(line, " ");
		all_col = strtok(NULL, " ");
  		//printf("\n%s\n", all_col);


		int col_count = 0;
  		do {
    			all_col = strtok(NULL, ",");
    			if(all_col)
			{ 
				//printf("|%s", all_col);
				strcpy (temp_full_columns[col_count].name_col, all_col);
				col_count ++;
				//all_col = NULL;
			}

  		} while(all_col);

		if (col_count == 0)
		{
			printf ("No columns specified.Exiting.\n");
			return -1;
		}


//removing optional whitespaces
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		int cc = 0;
		char *tcn;

		while (cc<col_count)
		{

			

			tcn = temp_full_columns[cc].name_col;
			//printf ("asdas5s :%s\n",tcn);
			int total_length = strlen(tcn);
			//algorithm to eliminate unncessary whitespaces in the beginning of the string
			//int index =0;
			int space_count = 0;

			
			
			while (tcn[space_count] == 32)
			{
				space_count++;
			}
				//printf ("The number of spaces in the beginning %d\n", space_count);
			if (space_count != 0)
			{
				int s_index =space_count;
				while (s_index<total_length || tcn[s_index] != 0)
				{
					tcn[s_index-space_count]=tcn[s_index];
					tcn[s_index] = 32;
					s_index++;
				}
			}

			//printf ("after beginning space elim:%s\n", bph_arga);

			//printf ("asdas5s :%s\n",tcn);

			strcpy (temp_full_columns[cc].name_col,tcn);
			cc++;

		}




		int sec_arga = 0;
		char *iph_arga;

		while (sec_arga<col_count)
		{

			iph_arga = temp_full_columns[sec_arga].name_col;
			//printf ("asdas5s :%s\n",tcn);
			//algorithm to eliminate unncessary whitespaces in the end of the string
			int space_count = strlen(iph_arga);
			do{

				if (iph_arga[space_count-1] == 32)
				{
					iph_arga[space_count-1] = 0;
				}

				space_count--;
			}while (iph_arga[space_count-1] == 32);

			strcpy (temp_full_columns[sec_arga].name_col,iph_arga);
			sec_arga++;
		}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			




		int l = 0;
		int flag = 0;

		while (l< col_count)
		{

			//printf ("The starting flag is %d\n", flag);

			

			strcpy (tables[table_count].columns[l].name_col, strtok(temp_full_columns[l].name_col,":"));
			strcpy (tables[table_count].columns[l].type, strtok(NULL,"\n"));


			//char *temp_type;
			//temp_type = strcpy(tables[table_count].columns[l].type, "[\0");

			//printf("The type is: %s\n",tables[table_count].columns[l].type);
			//printf("The type is: %s\n",temp_type);

			if (   (strcmp(tables[table_count].columns[l].type,"int") == 0)    ||
			       //(strcmp(tables[table_count].columns[l].type,"float") != 0)   &&
				(strcmp(tables[table_count].columns[l].type,"float") == 0)    )
			
			{
				//printf ("int or float, do not set flag\n");
				
				//return -1;
			} 
			else
			{
				//set flag
				flag++;
				//printf ("The first flag is %d\n", flag);
			}
				




			if (strcmp (strtok(tables[table_count].columns[l].type,"["), "char")==0)
			{
				char temp_len [MAX_TABLE_LEN];
				//printf ("It's a char!\n");
				strcpy (temp_len, strtok (NULL,"]"));
				tables[table_count].columns[l].int_length = atoi(temp_len);
				
				//printf ("The length is: %s\n",tables[table_count].columns[l].length);
				//printf ("The length is: %d\n",tables[table_count].columns[l].int_length);
				
			}
			else
			{
				//increment flag
				flag++;
				//printf ("The second flag is %d\n", flag);
				tables[table_count].columns[l].int_length = -1;
			}

			//printf ("The second flag is %d\n", flag);

			if (flag == 2)
			{
				printf ("Invalid Parameters.Exiting.\n");
				return -2;
			}
			else
			{
				//printf ("Do nothing.\n");
			}
				
				
			flag = 0;
		
				
	
			l++;
		}





		//print out configuration file information
		printf ("\n%s\n",tables[table_count].name);
		int k = 0;

		while (k < col_count)
		{

			
			printf ("%s", tables[table_count].columns[k].name_col);
			printf (":%s", tables[table_count].columns[k].type);
			printf (":%d", tables[table_count].columns[k].int_length);

			k++;
			printf ("\n");
		}

	
		tables[table_count].num_of_cols = col_count;
		printf ("There are %d columns.\n", tables[table_count].num_of_cols);
		table_count ++;
		totalNumberOfTables = table_count;


/*
		
		char *p;
		p = strtok(tables[table_count].columns[l].type, "[");

		int j = 0;
		while (j<col_count)
		{
			printf("%s\n",p);
			strcpy(p, strtok(tables[table_count].columns[l].type, "["));
			if (strcmp(p, "char"))
			{
				printf("Character found \n");
			}
		}
*/

/*

		int z = 0;
		while (z < col_count)
		{
			printf ("The columns for the previous table are: \n");
			printf ("%s\n", tables[table_count].columns[k].name_col);
			printf ("%s\n", tables[table_count].columns[k].type);
			z++;
		}	
		printf ("\n");
*/


	
	} else {
		// Ignore unknown config parameters.
	}

	return 0;
}


int read_config(const char *config_file, struct config_params *params)
{
	int error_occurred = 0;

	// Open file for reading.
	FILE *file = fopen(config_file, "r");
	if (file == NULL)
		error_occurred = 1;

	// Process the config file.
	while (!error_occurred && !feof(file)) {
		/// Read a line from the file.
		char line[MAX_CONFIG_LINE_LEN];
		char *l = fgets(line, sizeof line, file);

		int config_ok = 0;
		// Process the line.
		if (l == line)
		{
			config_ok = process_config_line(line, params);
			//printf ("The config_ok is: %d\n", config_ok);
			if (config_ok == -2) {error_occurred = 1;}

		}			
			
		else if (!feof(file))
		{
			error_occurred = 1;
		}
	}
	
	// if table names are specified in the configuration file, they need to created if they don't already exist
	if (table_count != 0)
	{
		// calls create_new to create non-existing files
		create_new(params);
		// resets table count to zero, so future reading of config files operate correctly
		table_count = 0;
	}
	// no tables were specified in the config files, nothing needs to done
	else
	{
		printf ("No tables specified.\n");
	}


	return error_occurred ? -1 : 0;
}
