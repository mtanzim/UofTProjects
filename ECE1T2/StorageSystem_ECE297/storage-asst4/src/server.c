/**
 * @file
 * @brief This file implements the storage server.
 *
 * The storage server should be named "server" and should take a single
 * command line argument that refers to the configuration file.
 * 
 * The storage server should be able to communicate with the client
 * library functions declared in storage.h and implemented in storage.c.
 *
 */
#include <assert.h>
#include <signal.h>
#include "utils.h"#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_LISTENQUEUELEN 20	///< The maximum number of queued connections.



//from echoselect.c
//#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold
#define MAX_TO_HANDLE 10
#define BUF_SIZE 1000




typedef struct cacheEntry cacheEntry;
//typedef struct allCaches allCaches;

struct predicates{
	char columnName[MAX_COLNAME_LEN];
	char operation[2];
	char value[MAX_VALUE_LEN];
	char type[10];
	
};

struct cacheEntry { //this struct is for each entry
	char key[51];
	char record[802];
	int version;
	int timesAccessed;
	int timeStamp;
};

struct allCaches {
	cacheEntry cache[MAX_RECORDS_PER_TABLE]; //TABLE as array of entries
	char tableName[MAX_TABLE_LEN];
	int numItems;
};

struct predicates predicate[MAX_COLUMNS_PER_TABLE];
struct allCaches totalCache[MAX_TABLES];

FILE* fp = NULL;
char config_f[MAX_CONFIG_LINE_LEN] = {0};
char* fileSearch = NULL;
	
int numberOfCaches;


/**
 * @brief Reverses string
 *
 * @param begin Specifies the start of the string to be reversed.
 * @param end Specifies the end of the string to be reversed.
 * 
 * @return Void function; reverses string
 *
 * Code copied from online resource found at:
 *
 * reference
 * [2]	"Stuart's Useful C/C++ Pages," [Online document],[cited 2009 Mar 1],
 *	Available HTTP: http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
 */
void strreverse(char* begin, char* end) {
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
	
}


/**
 * @brief Converts integers to strings
 *
 * @param value The integer to be converted
 * @param base Integer base (ie. decimal, binary, hexadecimal)
 * 
 * @return Void function; reverses string
 *
 * Code copied from online resource found at:
 *
 * reference
 * [2]	"Stuart's Useful C/C++ Pages," [Online document],[cited 2009 Mar 1],
 *	Available HTTP: http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
 */	
void itoa(int value, char* str, int base) {
	
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;
	
	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }
	
	// Take care of sign
	if ((sign=value) < 0) value = -value;
	
	// Conversion. Number is reversed.
	do *wstr++ = num[value%base]; while(value/=base);
	if(sign<0) *wstr++='-';
	*wstr='\0';
	
	// Reverse string
	strreverse(str,wstr-1);
}


/**
 * @brief measures time difference 
 *
 * @param result is the difference
 * @param X is the start_time
 * @param Y is the end_time
 * 
 * @return int ; status
 *
 * Code copied from online resource found at:
 *
 * reference
 * [1]	"21.2 Elapsed Time," [Online document],[cited 2009 Mar 24],
 *	Available HTTP: http://www.gnu.org/software/hello/manual/libc/Elapsed-Time.html
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     int
     timeval_subtract (result, x, y)
          struct timeval *result, *x, *y;
     {
       /* Perform the carry for the later subtraction by updating y. */
       if (x->tv_usec < y->tv_usec) {
         int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
         y->tv_usec -= 1000000 * nsec;
         y->tv_sec += nsec;
       }
       if (x->tv_usec - y->tv_usec > 1000000) {
         int nsec = (x->tv_usec - y->tv_usec) / 1000000;
         y->tv_usec += 1000000 * nsec;
         y->tv_sec -= nsec;
       }
     
       /* Compute the time remaining to wait.
          tv_usec is certainly positive. */
       result->tv_sec = x->tv_sec - y->tv_sec;
       result->tv_usec = x->tv_usec - y->tv_usec;
     
       /* Return 1 if result is negative. */
       return x->tv_sec < y->tv_sec;
     }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct timeval start_time, end_time, difference, totalProcessingTime;

//totalProcessingTime->tv_sec = 0;
//totalProcessingTime->tv_usec = 0;

//char OnOff[5] = {0};

/**
 * @brief Compares the user key input to the keys stored in the file to see 
 * if there is a match.
 *
 * @param sock The socket connected to the client.
 * @param _file A pointer to the current location in the file to be searched.
 * @param _key The key the function should search for.
 *
 * @return Returns 1 on success, 0 otherwise.
 *
 * Function reads 50 characters from the file into a character array to be 
 * compared with the key input. If a match is found the value corresponding
 * to the key is then read into another character array and copied into a 
 * response variable which is sent to the client side of the network through 
 * the socket connection following set communication protocol procedures. 
 */
int checkMatch (int sock, FILE* _file, char* _key, char* _txtTable, int size, char* on_off) {
printf("in checkMatch function\n");
        char keyVal[51] = {0};
        char value[801] = {0};
        char sendValue[801] = {0};
        char response [MAX_CMD_LEN];
        int i = 0; 
	
	char* fileKey = NULL;
	char* fileVersion = NULL;
	int version = 0;	
	
	// Reads the first 50 characters starting from the location of the 
        // current file pointer into the character array keyVal.
        while(i < 50) {
             keyVal[i] = fgetc(fp);
             i++; 
        }

	fileKey = strtok(keyVal, ".");
	fileVersion = strtok(NULL, ".");

	printf("fileKey is:%send\n", fileKey);
	printf("fileVersion is:%send\n", fileVersion);

	// String compares the value in keyVal to the user inputted key for 
        // a match.   
        if (strcmp(fileKey, _key) == 0) {

	    char temp = fgetc(fp);

	    // If a match is found, read the value corresponding to the key
	    // into a character array called value and send the information 
	    // to the client side of the network. 
            fgets(value, 800, fp);

	gettimeofday(&end_time, NULL);
	
	int time_works = timeval_subtract (&difference, &end_time, &start_time);

	totalProcessingTime.tv_sec = totalProcessingTime.tv_sec + difference.tv_sec;
	totalProcessingTime.tv_usec = totalProcessingTime.tv_usec + difference.tv_usec;

	printf ("Server processing time (item not in cache): %d.%d seconds.\n", 
					difference.tv_sec, difference.tv_usec); 

	printf("total processing time: %d.%d seconds.\n", totalProcessingTime.tv_sec, totalProcessingTime.tv_usec);

            strcpy(response, "OK ");
            strcat(response, value);

	    strcat(response, " ~");

	    char seconds[10];
	    itoa(totalProcessingTime.tv_sec, seconds, 10);

	    strcat(response, seconds);
	    strcat(response, ".");

	    char microsec[10];
	    itoa(totalProcessingTime.tv_usec, microsec, 10);

	    strcat(response, microsec);
 	    strcat(response, " ~");

	    strcat(response, " &");
	    strcat(response, fileVersion);
 	    strcat(response, " &");

            sendall(sock, response, strlen(response)); 
            sendall(sock, "\n", 1);  
            
            // Once the information has been sent, the file is closed and the 
            // function returns a 1 to indicate success.    
            fclose(fp);

//////////////////////////////////////////////// Replacing Items in the Cache ////////////////////////////////	        
	if (strcmp(on_off, "on") == 0) {
		int k, l = 0; 

		// Searches through all the caches to find the one that matches the user input table.
		for (k = 0; k < numberOfCaches; k++) {
		     printf("Table Name %d is %s\n", k, totalCache[k].tableName);

		     // Correct cache has been found.
		     if (strcmp(totalCache[k].tableName, _txtTable) == 0) {
			 printf("cache size is %d\n", size);

			 // If the cache is not full, add the new key/record pair to the end of the cache.
			 if (totalCache[k].numItems < size) {

		             printf("Cache is not full yet. Adding new search to the cache.\n");
			     strcpy(totalCache[k].cache[(totalCache[k].numItems)].key, keyVal);
			     strcpy(totalCache[k].cache[(totalCache[k].numItems)].record, value);
			     totalCache[k].cache[(totalCache[k].numItems)].timesAccessed = 1;
			     totalCache[k].cache[(totalCache[k].numItems)].version = atoi(fileVersion);
			     totalCache[k].numItems++;
			     return 1;
			 }

			 // Cache is full. Replace the least frequently accessed member. 

			 printf("The cache is full. Searching for least accessed key.\n");
			 int minTimesAccessed = totalCache[k].cache[0].timesAccessed;
			 int entryNumber = 0;

			 // Searches through all the entries of the cache for the least frequently accessed member.
			 for (l = 1; l < size; l++) {

			      // If key has never been accessed, replace it.
			      if (totalCache[k].cache[l].timesAccessed == 0) {

				  printf("Found a key that has never been accessed before.\n");
				  strcpy(totalCache[k].cache[l].key, keyVal);
				  strcpy(totalCache[k].cache[l].record, value);
				  totalCache[k].cache[l].timesAccessed = 1;
				  totalCache[k].cache[l].version = atoi(fileVersion);
				  return 1;
			      }

			      else {
				  if (totalCache[k].cache[l].timesAccessed < minTimesAccessed) {
				      entryNumber = l;
				  printf("Found a less accessed key:%s (%d)\n", totalCache[k].cache[l].key, 
										totalCache[k].cache[l].timesAccessed);
				  printf("Entry Number:%d\n", entryNumber);
				  }
			      }
			 }

			 printf("FINAL:\n\tLeast accessed key:%s\n\tLeast accessed record:%s\n\tTimes accessed:%d\n\n", 
				totalCache[k].cache[entryNumber].key, totalCache[k].cache[entryNumber].record, 
				totalCache[k].cache[entryNumber].timesAccessed);

		         strcpy(totalCache[k].cache[entryNumber].key, keyVal);
			 strcpy(totalCache[k].cache[entryNumber].record, value);
			 totalCache[k].cache[entryNumber].timesAccessed = 1;
			 totalCache[k].cache[entryNumber].version = atoi(fileVersion);

		printf("Values have now been replaced by:\n\tNew key:%s\n\tNew record:%s\n\tTimes accessed:%d\n\n", 
				totalCache[k].cache[entryNumber].key, totalCache[k].cache[entryNumber].record,
				totalCache[k].cache[entryNumber].timesAccessed);
			 return 1;
		     }
		}
	}
	return 1;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
        }
        
        // Return 0 if keys do not match.
        else {
            return 0;
        }    
}



/**
 * @brief Compares the user key input to the keys stored in the file to
 * see if there is a match. If so, the new input value overwrites the key's 
 * current corresponding value. 
 *
 * @param sock The socket connected to the client.
 * @param _file A pointer to the current location in the file to be searched.
 * @param _key The key the function should search forlowTemp.
 * @param _newValue The new value to be set at the input key location.
 *
 * @return Returns 1 on success, 0 otherwise.
 *
 * Function reads 50 characters from the file into a character array to 
 * be compared with the key input. If a match is found the new value
 * overwrites the exiting value corresponding to the key. A response is 
 * sent back to the client side of the network following set communication
 * protocol procedures. 
 *
 * Function also checks the boundary condition where the file is initially
 * empty. It then must write in the key as well as its corresponding value
 * in the first line of the file. 
 */
int writeMatch (int sock, FILE* _file, char* _key, char* _newValue, int counter, char* _txtTable) {

	char keyWithVersion[51] = {0};
	char keyVal[51] = {0};
	char wholeKey[51] = {0};

	char* justKey = NULL;
	char* fileKey = NULL;
	char* charVersion = NULL;
	char* fileVersion = NULL;
	int version = 0;
        char response [MAX_CMD_LEN];
        int i = 0;  

	strcpy(keyWithVersion, _key);

	justKey = strtok(_key, ".");
	charVersion = strtok(NULL, ".");

	version = atoi(charVersion); 

	printf("In writeMatch function...\njustKey:%send\ncharVersion:%send\nversion:%dend\n", 
		justKey, charVersion, version);

	printf("keyWithVersion after strtok:%s\n", keyWithVersion);

	// Reads the first 50 characters starting from the location of the 
        // current file pointer into the character array keyVal.
        while(i < 50) {

             keyVal[i] = fgetc(fp);
            
             // If fgetc reads a -1 then the end of the file has been reached. 
             // If this occurs the first time through the while loop (as noted
             // by the counter) then the file is initially empty. 
             if (keyVal[0] == -1 && counter < 50) {

		 strcat(justKey, ".1.");
		 int length = 51 - strlen(justKey);
	         int temp = 1;
 
    	    	 // Concatenates enough spaces to the end of the input key so that it
    	    	 // will be 50 characters long for comparison.
   	    	 while (temp < length) {
                  	strcat(justKey, " ");
                   	temp++;
            	 }	

                 // Write the key and its corresponding value into the first line
                 // of the file and send the appropriate message over the network.
		 fwrite(justKey, 1, 50, fp);
         	 fwrite(" ", 1, 1, fp);
        	 fwrite(_newValue, 1, 800, fp);
        	 
        	 gettimeofday(&end_time, NULL);
		 int time_works = timeval_subtract (&difference, &end_time, &start_time);
		 printf ("Server processing time (set): %d.%d seconds.\n",
								difference.tv_sec, difference.tv_usec); 
        	 
		 totalProcessingTime.tv_sec =  totalProcessingTime.tv_sec + difference.tv_sec;
		 totalProcessingTime.tv_usec = totalProcessingTime.tv_usec + difference.tv_usec;

		 printf ("Server processing time (item not in cache): %d.%d seconds.\n", 
					difference.tv_sec, difference.tv_usec); 

        	 strcpy(response, "OK New: ");
        	 strcat(response, _newValue);

		 strcat(response, " ~");

	    	 char seconds[10];
	    	 itoa(totalProcessingTime.tv_sec, seconds, 10);

	    	 strcat(response, seconds);
	    	 strcat(response, ".");

	    	 char microsec[10];
	    	 itoa(totalProcessingTime.tv_usec, microsec, 10);

	    	 strcat(response, microsec);
 		 strcat(response, " ~");

        	 sendall(sock, response, strlen(response)); 
        	 sendall(sock, "\n", 1); 
        	 	   
		 int z = 0;
       		    while (z<MAX_TABLES) {
	       	           // Finds the correct table in the table structure    
   	                   if (strcmp(tables[z].name, _txtTable) == 0) {
                               tables[z].fileCounter = counter;
			       break;
 	                   } 
	
	                   else {
	                       z++;
	                   }
	            } 

		 // Once the information has been sent, the file is closed and the 
            	 // function returns a 1 to indicate success.
		 fclose(fp); 
                 return 1;   
             }
                
             i++; 
             counter++;
        }

		    int z = 0;
       		    while (z<MAX_TABLES) {
	       	           // Finds the correct table in the table structure    
   	                   if (strcmp(tables[z].name, _txtTable) == 0) {
                               tables[z].fileCounter = counter;
			       printf("asdf\n");
			       break;
 	                   } 
	
	                   else {
				printf("asdfsdfasd\n");
	                       z++;
	                   }
	            } 

	strcpy(wholeKey, keyVal);
	printf("wholeKey:%s\n", wholeKey);	
	
	fileKey = strtok(keyVal, ".");
	fileVersion = strtok(NULL, ".");

	printf("fileKey is:%send\n", fileKey);
	printf("fileVersion is:%send\n", fileVersion);

	// String compares the value in keyVal to the user inputted key for 
        // a match.  
        if (strcmp(fileKey, justKey) == 0) {

	    if ((strcmp(charVersion, fileVersion) == 0) || version == 0) {
		 
		printf("fileVersion before incrementation:%s\n", fileVersion);

		int tempVersion = 0;
		tempVersion = atoi(fileVersion);
		tempVersion++;

		char writeVersion[10] = {0};

		itoa(tempVersion, writeVersion, 10);

		printf("writeVersion after incrementation:%s\n", writeVersion);

	        strcat(justKey, ".");
		strcat(justKey, writeVersion);
		strcat(justKey, ".");
		int length = 51 - strlen(justKey);
	        int temp = 1;

    	    	// Concatenates enough spaces to the end of the input key so that it
    	    	// will be 50 characters long for comparison.
   	    	while (temp < length) {
                       strcat(justKey, " ");
                       temp++;
            	}	

		printf("key to be written to file:%s\n", justKey);

		fseek(fp, -50, SEEK_CUR);

                // Write the key and its corresponding value into the first line
                // of the file and send the appropriate message over the network.
		fwrite(justKey, 1, 50, fp);
         	fwrite(" ", 1, 1, fp);
            	// If a match is found, overwrite the current value corresponding to 
            	// the key with the new value the function takes in. Send the information 
	    	// to the client side of the network after the process is completed. 
            	fwrite(_newValue, 1, 800, fp);
            
            	gettimeofday(&end_time, NULL);
	    	int time_works = timeval_subtract (&difference, &end_time, &start_time);
	    	printf ("Server processing time (set): %d.%d seconds.\n",
							difference.tv_sec, difference.tv_usec); 

            	totalProcessingTime.tv_sec = totalProcessingTime.tv_sec + difference.tv_sec;
	    	totalProcessingTime.tv_usec = totalProcessingTime.tv_usec + difference.tv_usec;

	    	printf ("Server processing time (item not in cache): %d.%d seconds.\n", 
					difference.tv_sec, difference.tv_usec); 

            	strcpy(response, "OK New: ");
            	strcat(response, _newValue);

	    	strcat(response, " ~");

	    	char seconds[10];
	    	itoa(totalProcessingTime.tv_sec, seconds, 10);

	    	strcat(response, seconds);
	    	strcat(response, ".");

	    	char microsec[10];
	    	itoa(totalProcessingTime.tv_usec, microsec, 10);

	    	strcat(response, microsec);
 	    	strcat(response, " ~");

            	sendall(sock, response, strlen(response)); 
            	sendall(sock, "\n", 1);  
            
	    	// Once the information has been sent, the file is closed and the 
            	// function returns a 1 to indicate success.
            	fclose(fp);
            	return 1; 
 	    } 

	    else {
		return -1;
	    }
	  
        }
        else {
    
            // Return 0 if keys do not match.
            return 0;
        }    
}



/**
 * @brief Compares the user key input to the keys stored in the file to see 
 * if there is a match. If so, the key and its corresponding value are 
 * deleted from the file. 
 *
 * @param sock The socket connected to the client.
 * @param _file A pointer to the current location in the file to be searched.
 * @param _key The key the function should search for.
 *
 * @return Returns 1 on success, 0 otherwise.
 *
 * Function reads 50 characters from the file into a character array to be 
 * compared with the key input. If a match is found the file pointer is moved 
 * back 50 places to the start of the key value and the entire line is 
 * overwritten with white space. A delete message is sent to the client side 
 * of the network through the socket connection following appropriate 
 * communication protocol procedures. 
 */
int deleteMatch (int sock, FILE* _file, char* _key) {
printf("deletematch function\n");
	char keyWithVersion[51] = {0};
	char keyVal[51] = {0};
	char wholeKey[51] = {0};
	char* justKey = NULL;
	char* fileKey = NULL;
	char* charVersion = NULL;
	char* fileVersion = NULL;
	int version = 0;
        char response [MAX_CMD_LEN];
        int i = 0;  

	strcpy(keyWithVersion, _key);

	justKey = strtok(_key, ".");
	charVersion = strtok(NULL, ".");

	version = atoi(charVersion); 

	printf("In deleteMatch function...\njustKey:%send\ncharVersion:%send\nversion:%dend\n", 
		justKey, charVersion, version);

	printf("keyWithVersion after strtok:%s\n", keyWithVersion);

	// Reads the first 50 characters starting from the location of the 
        // current file pointer into the character array keyVal.
        while(i < 50) {
              keyVal[i] = fgetc(fp);
              i++; 
        }
	
	strcpy(wholeKey, keyVal);
	printf("wholeKey:%s\n", wholeKey);	
	
	fileKey = strtok(keyVal, ".");
	fileVersion = strtok(NULL, ".");

	printf("fileKey is:%send\n", fileKey);
	printf("justKey is:%send\n", justKey);
	printf("keyVal is:%send\n", keyVal);
	printf("fileVersion is:%send\n", fileVersion);

	// String compares the value in keyVal to the user inputted key for 
        // a match.  
        if (strcmp(fileKey, justKey) == 0) {

	    printf("in deletematch....version:%d\n", version);
	    printf("in deletematch....charVersion:%s\n", charVersion);
	    printf("in deletematch....fileVersion:%d\n", fileVersion);

	    if ((strcmp(charVersion, fileVersion) == 0) || version == 0) {

	    	printf("Match found! (delete)\n");            

            	// If a match is found, back up the file pointer 50 character and 
            	// overwrite the entire like with white space. Send the information 
            	// to the client side of the network. 
            	fseek(fp, -50, SEEK_CUR);

            	int i = 0;
            	while(i<851) {
                      fwrite(" ", 1, 1, fp);
                      i++;
            	} 

		gettimeofday(&end_time, NULL);
		int time_works = timeval_subtract (&difference, &end_time, &start_time);
		printf ("Server processing time (delete): %d.%d seconds.\n",
						difference.tv_sec, difference.tv_usec); 

		totalProcessingTime.tv_sec += difference.tv_sec;
		totalProcessingTime.tv_usec += difference.tv_usec;

		printf ("Server processing time (item not in cache): %d.%d seconds.\n", 
					difference.tv_sec, difference.tv_usec); 

            	strcpy(response, "OK Deleted ");
            	strcat(response, _key);

	    	strcat(response, " ~");

	    	char seconds[10];
	    	itoa(totalProcessingTime.tv_sec, seconds, 10);

	    	strcat(response, seconds);
	    	strcat(response, ".");

	    	char microsec[10];
	    	itoa(totalProcessingTime.tv_usec, microsec, 10);

	    	strcat(response, microsec);
 	    	strcat(response, " ~");
            	sendall(sock, response, strlen(response)); 
            	sendall(sock, "\n", 1);  
            
		printf("sent info\n");
		
	    	// Once the information has been sent, the file is closed and the 
            	// function returns a 1 to indicate success.   
            	fclose(fp);
            	return 1; 
	    } 
	    else {
		return -1;
	    }  
        }

        else {

		printf("returning 0!\n");
 
            // Return 0 if keys do not match.
            return 0;
        }    
}


/**
 * @brief Checks to see whether the columns passed in are valid and if their
 * corresponding values match the types specified in the configuration file.
 *
 * @param sock The socket connected to the client.
 * @param checkNum Specifies which predicate is being checked for validity.
 * @param _txtTable The name of the table the columns are found in.
 *
 * @return Returns 1 on success, 0 otherwise.
 *
 * [1]  "Re: check if input is int float or char," [Online document],[cited 2009 Feb 20],
 *       Available HTTP: http://forum.codecall.net/c-c/8924-check-if-input-int-float-char.html
 */
int checkExistence (int checkNum, int sock, char* _txtTable) {

	char response [MAX_CMD_LEN];
	int tableExists = 0;
	int columnExists = 0;
	int b = 0;
	int c = 0;

        while (b<MAX_TABLES) {
                  
	       // Finds the correct table in the table structure    
   	       if (strcmp(tables[b].name, _txtTable) == 0) {
	           tableExists = 1;
	            
		   // Searches through the columns in the table to make sure
		   // input columns are in the correct order       
		   while (c < tables[b].num_of_cols) {

	       		  char temp_col[MAX_CMD_LEN] = {0};
	       		  strcpy(temp_col, tables[b].columns[c].name_col);

	       		  if (strcmp(temp_col, predicate[checkNum].columnName) == 0) {
			      columnExists = 1;
		   	      break;
	                  }

	       		  else {
			      c++;
	                  }
                   }
                   break;
 	       } 
	
	       else {
	           b++;
	       }
	} 
	
	// Table could not be found.
        if (tableExists == 0) {
            strcpy(response, "ERROR TableNotFound\n");
            sendall(sock, response, strlen(response));
	    fclose(fp);
            return -1;
	}

	// Column does not exist.
        else if (columnExists == 0) {
            strcpy(response, "ERROR ColumnNotFound\n");
            sendall(sock, response, strlen(response));
	    fclose(fp);
            return -1;
	}	    
	
	else {
		
	    char actual_type[10] = {0};	
	    char temp_type[MAX_CMD_LEN] = {0}; 
            strcpy(temp_type, tables[b].columns[c].type);
                                  
	    int idx = 0;
            char str[strlen(predicate[checkNum].value)];
	    strcpy(str, predicate[checkNum].value);					

	    int numbersfound = 0;
	    int dotsfound = 0;
	    int signsfound = 0;

	    // Checks 	totalProcessingTime.tv_sec += difference.tv_sec;
	totalProcessingTime.tv_usec += difference.tv_usec;

	printf ("Server processing time (item not in cache): %d.%d seconds.\n", 
					difference.tv_sec, difference.tv_usec); 
	    // Modified version of the code referenced at the end of the function description
	    for (idx = 0; (idx<strlen(predicate[checkNum].value)) && (str[idx]); idx++){
 		 printf("please go into the for loop!\n");
		 numbersfound += ((str[idx] >= '0') && (str[idx] <= '9'));
		 dotsfound += (str[idx] == '.');
		 signsfound += ((str[idx] == '+') || (str[idx] == '-'));
	    }

	    // Integers consist of all numbers or all numbers and a sign. 
	    if (numbersfound == strlen(predicate[checkNum].value) || 
		((numbersfound == strlen(predicate[checkNum].value)-1) && (signsfound == 1))) {
		printf("integer\n");
                strcpy(actual_type, "int");
	    }

	    // Floats consist of a dot and all numbers or a dot, a sign and all numbers.
	    else if (((dotsfound == 1) && (numbersfound == strlen(predicate[checkNum].value)-1))
		|| ((dotsfound == 1) && (numbersfound == strlen(predicate[checkNum].value)-2) 
		&& (signsfound ==1))) {
  		printf("floating point\n");
                strcpy(actual_type, "float");
	    }

	    // If value is neither a float or an int, then it is assumed to be a string.
            else {
               	printf("string\n");
                strcpy(actual_type, "char");
	    }

            // The value entered is of the correct type. Set the type variable in the predicates
	    // structure accordingly.
	    if (strcmp(actual_type, temp_type) == 0) {

		strcpy(predicate[checkNum].type, actual_type);

		if (strcmp(actual_type, "int") == 0 || strcmp(actual_type, "float") == 0) {
		    int a = strcmp(predicate[checkNum].operation, "=");
		    int b = strcmp(predicate[checkNum].operation, "<");
		    int c = strcmp(predicate[checkNum].operation, ">");
		
		    if (a == -1 && b == -1 && c == -1) {
			printf("Invalid operation for type int and float!\n");		
			return 0;
		    }
		}

		else if (strcmp(actual_type, "char") == 0) {
		    int d = strcmp(predicate[checkNum].operation, "=");
		 
		    if (d == -1) {
			printf("invalid operation for type char\n");	
			return 0;
		    }
		}
		return 1;
      	    }
	
	    else{
                return 0;
	    }
	}
}


/**
 * @brief Finds the column from the file and extracts its corresponding value.
 *
 * @param fileValue The string containing all the columns and values for 1 key.
 * @param num Helps specify which column is being searched for.
 *
 * @return Returns the value corresponding to the column on success.
 *
 */
char* columnFinder (char* fileValue, int num) {

	int timesThroughLoop = 0;
	fileSearch = strtok(fileValue, ":");

	// Loops through the input string for the specific column.
	while(fileSearch != NULL) {

	      if (timesThroughLoop != 0) {
		  fileSearch = strtok(NULL, ":");
	      }

	      // Break out of loop once column is found.      
	      if (strcmp(predicate[num].columnName, fileSearch) == 0) {
		printf("yay\n");
		  break;
	      }

	      fileSearch = strtok(NULL, ",");
	      timesThroughLoop++;
	}

	timesThroughLoop = 0;
        fileSearch = strtok(NULL, ",");

	// Returns the value following the column name. 
	return fileSearch;
}




/**
 * @brief Process a command from the client.
 *
 * @param sock The socket connected to the client.
 * @param cmd The command received from the client.
 * @param params The structure where config parameters are loaded.
 *
 * @return Returns 0 on success, -1 otherwise.
 */ 

int handle_command(int sock, char *cmd, struct config_params *params)
{
	LOG(("Processing command '%s'\n", cmd));


printf("cache policy:%s\ncache size:%d\n", params->cache_policy, params->cache_size);

	char table[MAX_CMD_LEN] = {0};
	char key[MAX_CMD_LEN] = {0};
	char keyWithVersion[MAX_CMD_LEN] = {0};
	char newValue[MAX_CMD_LEN] = {0};
	char tempNewValue[MAX_CMD_LEN] = {0};

	char response[MAX_CMD_LEN] = {0};
        char justTable[MAX_CMD_LEN] = {0};
	char txtTable[MAX_CMD_LEN] = {0};

	int maxKeys = 0;
	char temp_version[10];
	int version = 0;
				
	char actual_type[10] = {0};

        // Parses the command received based on the format of the command.
        // If it begins with GET the next two values should correspond to 
        // the table name and key value respectively. SET is followed by 
        // the table name, key, and new value to be stored.  
	 char skip[MAX_CMD_LEN] = {0};

	int fields = sscanf(cmd, "GET %s %s", table, key); 

	int fields2 = sscanf(cmd, "SET %s %s %s %s", table, key, temp_version, tempNewValue);
	
	printf("version before atoi:%s\n", temp_version);

	version = atoi(temp_version);

	printf("version:%d\n", version);

	int fields3 = sscanf(cmd, "FIND %s", table);

	strcpy(justTable, table);


	// Concatenates ".txt" to the end of the table name for file opening. 
	strcat(table, ".txt");
	
	strcpy(txtTable, table);

	// Sets file_path to the full directory
	char file_path[MAX_PATH_LEN + MAX_TABLE_LEN]= "\0";
	strcat (file_path, params->data_directory);
	strcat (file_path, "/");
	strcat (file_path, table);

	strcpy(table, file_path);

	// If the command is GET and enough inputs are passed in to perform
        // the operation, do the following: 
	if (fields == 2) {

    	 /*   int length = 51 - strlen(key);
            int temp = 1;
 
    	    // Concatenates enough spaces to the end of the input key so that it
    	    // will be 50 characters long for comparison.
    	    while (temp < length) {
                   strcat(key, " ");
                   temp++;
            }	*/	

	    // Checks to see if file exists by reading through the table names in 
	    // the specified configuration file.
	    FILE* cf;
	    cf = fopen (config_f, "r");
	    int table_found = 0;
	    

	    if (cf!=NULL) {

		// Checks through the entire configuration file for the table name. 
		while (!feof(cf)) {
			
			char line[MAX_CONFIG_LINE_LEN];
			char *l = fgets(line, sizeof line, cf);

			// Process the line.
			if (l == line) {

			    // Extract config parameter name and value.
			    char name[MAX_CONFIG_LINE_LEN];
			    char value[MAX_CONFIG_LINE_LEN];
			    sscanf(line, "%s %s\n", name, value);

			    // If the table name is found in the configuration file
			    // then it exists and  table_found is set to 1. 
			    if (strcmp(name, "table") == 0) {
				if (strcmp(value, justTable) == 0) {
				    table_found =1;
				    break;
				}
			    }
			}			
		}
		fclose(cf);
	    } 

	    // If the table is not in the configuration file, command cannot be 
 	    // processed. Send back the appropriate error message. 
	    if (table_found == 0){
	        strcpy(response, "ERROR TableNotFound\n");
      	       	sendall(sock, response, strlen(response));
                return -1;
	    }


    	    // Otherwise file containing table is opened. 
            else {

//////////////////////////////////////////////// GET: Check through Cache first ///////////////////////////////
printf("before\n");

gettimeofday(&start_time, NULL);

if (strcmp(params->cache_policy, "on") == 0) {	
	
		int i, j = 0; 
printf("after\n");
		// Searches through all the caches to find the one that matches the user input table.
		for (i = 0; i < numberOfCaches; i++) {
		     printf("Table Name %d is %s\n", i, totalCache[i].tableName);

		     // Correct cache has been found.
		     if (strcmp(totalCache[i].tableName, txtTable) == 0) {
			 printf("cache size is %d\n", params->cache_size);

			 // Searches through all the entries of the cache for a matching key.
			 for (j = 0; j < params->cache_size; j++) {
			      printf("key %d in table %d is %s\n", j, i, totalCache[i].cache[j].key);

			      // Key has been found.
			      if (strcmp(totalCache[i].cache[j].key, key) == 0) {
				
				gettimeofday(&end_time, NULL);
				int time_works = timeval_subtract (&difference, &end_time, &start_time);
				printf ("Server processing time (item found in cache): %d.%d seconds.\n",
								difference.tv_sec, difference.tv_usec); 

				totalProcessingTime.tv_sec += difference.tv_sec;
				totalProcessingTime.tv_usec += difference.tv_usec;

				printf ("Server processing time (item not in cache): %d.%d seconds.\n", 
									difference.tv_sec, difference.tv_usec); 
				
				  // Increment the total number of times the key has been accessed.
				  totalCache[i].cache[j].timesAccessed ++;
				  printf("times accessed:%d\n", totalCache[i].cache[j].timesAccessed);

				  // Return the record corresponding to the key back to the client
				  // side of the network following appropriate communication protocol
				  // procedures.
			     	  strcpy(response, "OK ");
            		     	  strcat(response, totalCache[i].cache[j].record);
				  strcat(response, " ~");

				  char seconds[10];
				  itoa(totalProcessingTime.tv_sec, seconds, 10);

				  strcat(response, seconds);
				  strcat(response, ".");

				  char microsec[10];
				  itoa(totalProcessingTime.tv_usec, microsec, 10);

				  strcat(response, microsec);
			 	  strcat(response, " ~");

				  strcat(response, " &");

				  char tempV[10] = {0};
				  itoa(totalCache[i].cache[j].version, tempV, 10);

	    			  strcat(response, tempV);
 	    			  strcat(response, " &");

	           		  sendall(sock, response, strlen(response));
            		     	  sendall(sock, "\n", 1); 
                 	 	  return 0;
			      }
			 }
		     }
		}
}
		printf("Nothing should reach here if item is in the cache!\n");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	       // If the key was not found in the cache, proceed to regular file retrievals.
		
    	       // Opens the file for reading.
      	       fp = fopen(table, "r");

	       int tempSize = params->cache_size;

	       // Check if the specified key can be found in the file by calling
               // the checkMatch function defined above. 
       	       int check = checkMatch(sock, fp, key, txtTable, tempSize, params->cache_policy);
     
	       // Keep looping if checkMatch returns 0 indicating the key has not
               // yet been found. 
               while (check == 0) {

		      // Move the file pointer 801 spaces to skip over the value 
                      // field before calling checkMatch again to compare the
                      // next key in the file. 
               	      char skipValue[802] = {0};
              	      fgets(skipValue, 801, fp);
              	      int temp2 = fgetc(fp);
              	      int temp3 = fgetc(fp);

                      check = checkMatch(sock, fp, key, txtTable, tempSize, params->cache_policy); 
 
                      if(feof(fp)) {
			
			gettimeofday(&end_time, NULL);
			int time_works = timeval_subtract (&difference, &end_time, &start_time);
			printf ("Server processing time (item not found): %d.%d seconds.\n",
								difference.tv_sec, difference.tv_usec); 
			
			totalProcessingTime.tv_sec += difference.tv_sec;
			totalProcessingTime.tv_usec += difference.tv_usec;

			printf ("Server processing time (item not in cache): %d.%d seconds.\n", 
					difference.tv_sec, difference.tv_usec); 

			 // If the end of the file has been reached and checkMatch
                         // still has not found the key, then output error message
                         // that the key was not found. 
                 	 strcpy(response, "ERROR KeyNotFound");

	    strcat(response, " ~");

	    char seconds[10];
	    itoa(totalProcessingTime.tv_sec, seconds, 10);

	    strcat(response, seconds);
	    strcat(response, ".");

	    char microsec[10];
	    itoa(totalProcessingTime.tv_usec, microsec, 10);

	    strcat(response, microsec);
 	    strcat(response, " ~");
                 	 sendall(sock, response, strlen(response)); 
                 	 sendall(sock, "\n", 1); 
                 	 fclose(fp); 
                 	 return -1;
                      }
      	       }
            return 0; 
    	    }
	}
        // End of operation for retrieving from file.

    
     	// Otherwise if the command is SET and enough inputs are passed in to perform
        // the operation, do the following: 
	else if (fields2 == 4) {

	    strcpy(keyWithVersion, key);
	    strcat(keyWithVersion, ".");
	    strcat(keyWithVersion, temp_version);
	    strcat(keyWithVersion, ".");

	    printf("key with version:%s\n", keyWithVersion);

	    int length = 51 - strlen(keyWithVersion);
	    int temp = 1;
 
    	    // Concatenates enough spaces to the end of the input key so that it
    	    // will be 50 characters long for comparison.
   	    while (temp < length) {
                   strcat(keyWithVersion, " ");
                   temp++;
            }	

	    // Checks to see if file exists by reading through the table names in 
	    // the specified configuration file.
	    FILE *cf = fopen (config_f, "r");
	    int table_found2 = 0;
	    if (cf!=NULL) {

		// Checks through the entire configuration file for the table name. 
		while (!feof(cf)) {
			
			char line[MAX_CONFIG_LINE_LEN];
			char *l = fgets(line, sizeof line, cf);

			// Process the line.
			if (l == line) {

			    // Extract config parameter name and value.
			    char name[MAX_CONFIG_LINE_LEN];
			    char value[MAX_CONFIG_LINE_LEN];
			    sscanf(line, "%s %s\n", name, value);
	
			    // If the table name is found in the configuration file
			    // then it exists and table_found is set to 1. 
			    if (strcmp(name, "table") == 0) {
				if (strcmp(value, justTable) == 0) {
				    table_found2 = 1;
				    break;
				}
			    }
			}		
		}
		fclose(cf);
	    } 
	
	    // If the table is not in the configuration file, command cannot be 
 	    // processed. Send back the appropriate error message. 
	    if (table_found2 == 0){
		strcpy(response, "ERROR TableNotFound\n");
      	       	sendall(sock, response, strlen(response));
                return -1;
	    }

	
            // Otherwise file containing table is opened. 
    	    else {
    	
    	        // Next step: Open the file for reading and writing. 
    	        fp = fopen(table, "r+");
	
		// Checks the case when the user wants to delete a key/value pair.
       	        if (strcmp(tempNewValue, "DELETE") == 0) {
gettimeofday(&start_time, NULL);

//////////////////////////////////////////////// Delete Value in Cache ///////////////////////////////////////
if ((strcmp(params->cache_policy, "on") == 0)) {
		printf("Deleting the value in the cache.\n");

		int i, j, p, q, r = 0; 

		// Searches through all the caches to find the one that matches the user input table.
		for (i = 0; i < numberOfCaches; i++) {
		     printf("Table Name %d is %s\n", i, totalCache[i].tableName);

		     // Correct cache has been found.
		     if (strcmp(totalCache[i].tableName, txtTable) == 0) {
			 printf("Cache size is %d\n", params->cache_size);

			 // Searches through all the entries of the cache for a matching key.
			 for (j = 0; j < params->cache_size; j++) {
			      printf("key %d in table %d is %s\n", j, i, totalCache[i].cache[j].key);

			      // Key has been found.
			      if (strcmp(totalCache[i].cache[j].key, key) == 0) {
				  printf("Key found:%s\nPrevious record is:%s\n", totalCache[i].cache[j].key, 
				  totalCache[i].cache[j].record);

				  if ((version == 0) || (version == totalCache[i].cache[j].version)) { 
				      if (j == ((params->cache_size) - 1)) {

				          for (p = 0; p < 40; p++) {					
					       totalCache[i].cache[j].key[p] = 0;
				          }

				          for (q = 0; q < 802; q++) { 
					       totalCache[i].cache[j].record[q] = 0;
				          }

				          totalCache[i].cache[j].timesAccessed = 0;
			                  totalCache[i].cache[j].version = 0;
				          totalCache[i].numItems--;
				      }

				      else {

				          for (r = j; r < params->cache_size; r++) {
	
  				               strcpy(totalCache[i].cache[r].key, totalCache[i].cache[r+1].key);
				               strcpy(totalCache[i].cache[r].record, totalCache[i].cache[r+1].record);
				               totalCache[i].cache[r].timesAccessed = 
										totalCache[i].cache[r+1].timesAccessed;
				               totalCache[i].cache[r].version = totalCache[i].cache[r+1].version;
				          }
				          totalCache[i].numItems--;

                                      }
				  }
			      }
			 }
		     }
		}

		printf("THE NEW CACHE AFTER A DELETE\n");

		int x = 0;
		int y = 0;

		for (x = 0; x < totalNumberOfTables; x++) {
			for (y = 0; y < params->cache_size; y++) {
				printf("key %d is %s\n", y, totalCache[x].cache[y].key);
				printf("record %d is %s\n", y, totalCache[x].cache[y].record);
			}
			printf("Name of Each Cache:%s\n", totalCache[x].tableName);
			printf("Total number of items in each cache:%d\n", totalCache[x].numItems);
		}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	      	   
		    // Check if the specified key can be found in the file (and 
		    // subsequently deleted from the file) by calling the 
		    // deleteMatch function defined above. 

	    	    char tempkVersion[MAX_CMD_LEN] = {0};
		    strcpy(tempkVersion, keyWithVersion);

       		    int check = deleteMatch(sock, fp, tempkVersion);

		    printf("check is:%d\n", check);

	            if (check == -1) {
			printf("go here?\n");
			strcpy(response, "ERROR TransactionAbort\n");
        		sendall(sock, response, strlen(response));
	  		fclose(fp);
			return -1;
		    }

     	            // Keep looping if deleteMatch returns 0 indicating the key has 
                    // not yet been found and deleted.
       		    while (check == 0) {
			
                           printf("in check is 0\n");              	           

 			   // Move the file pointer 801 spaces to skip over the value 
                           // field before calling deleteMatch again to compare the
                           // next key in the file.
		           char skipValue[802] = {0};
              	           fgets(skipValue, 801, fp);
              	           int temp2 = fgetc(fp);
              	           int temp3 = fgetc(fp);

	    		   char tempkVersion[MAX_CMD_LEN] = {0};
		    	   strcpy(tempkVersion, keyWithVersion);

			   printf("call deleteMatch again\n");
		
              	           check = deleteMatch(sock, fp, tempkVersion); 

 	           	   if (check == -1) {
			       strcpy(response, "ERROR TransactionAbort\n");
        		       sendall(sock, response, strlen(response));
	  		       fclose(fp);
			       return -1;
		           }

              	           if(feof(fp)) {

				gettimeofday(&end_time, NULL);
				int time_works = timeval_subtract (&difference, &end_time, &start_time);
				printf ("Server processing time (item not found): %d.%d seconds.\n",
								difference.tv_sec, difference.tv_usec); 
				totalProcessingTime.tv_sec += difference.tv_sec;
				totalProcessingTime.tv_usec += difference.tv_usec;

				printf ("Server processing time (item not in cache): %d.%d seconds.\n", 
					difference.tv_sec, difference.tv_usec); 

								
			      // If the end of the file has been reached and deleteMatch
                              // still has not found the key, then output error message
                              // that the key was not found. 
                 	      strcpy(response, "ERROR KeyNotFound");

	    strcat(response, " ~");

	    char seconds[10];
	    itoa(totalProcessingTime.tv_sec, seconds, 10);

	    strcat(response, seconds);
	    strcat(response, ".");

	    char microsec[10];
	    itoa(totalProcessingTime.tv_usec, microsec, 10);

	    strcat(response, microsec);
 	    strcat(response, " ~");
                 	      sendall(sock, response, strlen(response)); 
                 	      sendall(sock, "\n", 1); 
                 	      fclose(fp); 
                 	      return -1;
                           }
                    } 

		printf("reach here = no seg fault\n");
                return 0;
                }
  		
		// Else do error checking before writing to the file.
		else {                
		    char *value = strtok(cmd, " ");

		    // Skips the first 3 inputs in the command to get to the predicate values.
                    int a = 0;
                    while (a < 4) {
                           strcat(skip, value);
                           value = strtok(NULL, " ");
                           a++;
                    }

		    printf("Value:%s\n", value);
		    int tableExists = 0;
            	    while (value != NULL) {
			   int b = 0;
                           while (b<MAX_TABLES) {

				  // Finds the correct table to search through.
				  if (strcmp(tables[b].name, txtTable) == 0) {
				      tableExists = 1;
				      int c = 0;
				printf("number of columns:%d\n", tables[b].num_of_cols);
				      // Finds the specific column in the table.
			    	      while (c < tables[b].num_of_cols) {

				             char temp_col[MAX_CMD_LEN] = {0};
					     strcpy(temp_col, tables[b].columns[c].name_col);

				             // If column in in the right order, concatenate a colon 
					     // before reading for the column value.
					     if (strcmp(temp_col, value) == 0) {
						 printf("Column name matches!\n");
						 strcat(newValue, value);
						 strcat(newValue, ":");
					     }

					     // If column is not found, output error message.
					     else if (strcmp(temp_col, value) != 0) {
						 strcpy(response, "ERROR InvalidParameter\n");
      	       		    			 sendall(sock, response, strlen(response));
	  		    			 fclose(fp);
          	            			 return -1;
					     }

					     // Reads the value corresponding to the column for error
					     // checking.
              	            		     printf("The value is: %s\n", value);
	                    		     value = strtok(NULL, ",");

					     char temp_type[MAX_CMD_LEN] = {0}; 
 					     strcpy(temp_type, tables[b].columns[c].type);
                                  
					     int idx = 0;
                			     char str[strlen(value)];
					     strcpy(str, value);						

			        	     int numbersfound = 0;
					     int dotsfound = 0;
					     int signsfound = 0;
					
					     // Checks whether the value corresponding to the column name 
					     // is of a valid type. Modified version of the code referenced 
					     // at [1].
					     for (idx = 0; (idx<strlen(value)) && (str[idx]); idx++){
						  numbersfound += ((str[idx] >= '0') && (str[idx] <= '9'));
						  dotsfound += (str[idx] == '.');
						  signsfound += ((str[idx] == '+') || (str[idx] == '-'));
					     }

					     // Integers consist of all numbers or all numbers and a sign.
					     if ((numbersfound == strlen(value)) || 
						 ((numbersfound == strlen(value)-1) && (signsfound == 1))) {
					         printf("integer\n");
                                                 strcpy(actual_type, "int");
					     }
				
					     // Floats consist of a dot and all numbers or a dot, a sign 
					     //and all numbers.
					     else if (((dotsfound == 1) && (numbersfound == strlen(value)-1))
						 || ((dotsfound == 1) && (numbersfound == strlen(value)-2) && 
						 (signsfound ==1))) {
  					         printf("floating point\n");
                                                 strcpy(actual_type, "float");
					     }

					     // If value is neither a float or an int, then it is assumed to 
					     // be a string.
					     else {
               					 printf("string\n");
                                                 strcpy(actual_type, "char");
					     }

 					     // The value entered is of the correct type. Concatenate the value 
					     // after the column name to be written to file.
					     if (strcmp(actual_type, temp_type) == 0) {
						 printf("actual type = temp type!\n");
						 strcat(newValue, value);


		            	                 strcat(newValue, ","); 
        	            			 value = strtok(NULL, " ");
						 c++;

						// if (c == tables[b].num_of_cols) {
 						  //   b++;
						 //}
					     }

					     // If the type did not match, the invalid parameter message is returned.
					     else {
						 strcpy(response, "ERROR InvalidParameter\n");
      	       		    			 sendall(sock, response, strlen(response));
	  		  		 	 fclose(fp);
          	            			 return -1;
					     }
				      }
				  break;
 				  } 
	
				  else {
				      b++;
				  }
                           } 

			   // Table could not be found.
                           if (tableExists == 0) {
			       strcpy(response, "ERROR TableNotFound\n");
        	       	       sendall(sock, response, strlen(response));
	  		       fclose(fp);
          	               return -1;
			   }
	            } 

	    	    int length2 = 801 - strlen(newValue);
	    	    int temp2 = 1;
    	    	 
      		    // Concatenates enough spaces to the end of the input value so that it
    	   	    // will be 800 characters long for consistency. Unless the input
            	    // value field is NULL. 
    	    	 
		    if (newValue != NULL) {
              	        while (temp2 < length2) {
             	 	       strcat(newValue, " ");
             	               temp2++;
               	        }
            	    }

	  	    int z = 0;
       		    while (z<MAX_TABLES) {
	       	           // Finds the correct table in the table structure    
   	                   if (strcmp(tables[z].name, txtTable) == 0) {
				printf("table name:%s\n", tables[z].name);
				printf("table counter:%d\n", tables[z].fileCounter);
                               break;;
 	                   } 
	
	                   else {
	                       z++;
	                   }
	            } 

		    printf("after loop:%d\n", tables[z].fileCounter);

gettimeofday(&start_time, NULL);

//////////////////////////////////////////////// Update Value in Cache ///////////////////////////////////////
if (strcmp(params->cache_policy, "on") == 0) {
		printf("Updating the value in the cache.\n");

		int i, j = 0; 

		// Searches through all the caches to find the one that matches the user input table.
		for (i = 0; i < numberOfCaches; i++) {
		     printf("Table Name %d is %s\n", i, totalCache[i].tableName);

		     // Correct cache has been found.
		     if (strcmp(totalCache[i].tableName, txtTable) == 0) {
			 printf("Cache size is %d\n", params->cache_size);

			 // Searches through all the entries of the cache for a matching key.
			 for (j = 0; j < params->cache_size; j++) {
			      printf("key %d in table %d is %s\n", j, i, totalCache[i].cache[j].key);

			      // Key has been found.
			      if (strcmp(totalCache[i].cache[j].key, key) == 0) {
				  printf("Key found:%s\nPrevious record is:%s\n", totalCache[i].cache[j].key, 
				  totalCache[i].cache[j].record);
				  // Increment the total number of times the key has been accessed.
				  //totalCache[i].cache[j].timesAccessed ++;
				  //printf("times accessed:%d\n", totalCache[i].cache[j].timesAccessed);

			          if ((version == 0) || (version == totalCache[i].cache[j].version)) {
				      strcpy(totalCache[i].cache[j].record, newValue);
				      totalCache[i].cache[j].version++;
				      printf("New record is:%s\n", totalCache[i].cache[j].record);
   				      printf("Version in the cache is: %d", totalCache[i].cache[j].version);
			          }
			      }
			 }
		     }
		}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


	            // Check if the specified key can be found in the file so that its 
		    // corresponding value can be updated by calling the writeMatch 
		    // function defined above. 
		    char tempkVersion[MAX_CMD_LEN] = {0};

		    printf("keyWithVersion(1):%s\n", keyWithVersion);
		    strcpy(tempkVersion, keyWithVersion);

		    printf("tempkVersion(1):%s\n", tempkVersion);

                    int check = writeMatch(sock, fp, tempkVersion, newValue, tables[z].fileCounter, txtTable);
     
		    printf("counter:%d\n", tables[z].fileCounter);

		    printf("check is:%d\n", check);

		    if (check == -1) {
		        strcpy(response, "ERROR TransactionAbort\n");
        	       	sendall(sock, response, strlen(response));
	  		fclose(fp);
          	        return -1;
		    }

     	            // Keep looping if writeMatch returns 0 indicating the key has 
                    // not yet been found and updated.
       		    while (check == 0) {

 		           // Move the file pointer 801 spaces to skip over the value 
                           // field before calling writeMatch again to compare the
                           // next key in the file.
              		   char skipValue[802] = {0};
              	  	   fgets(skipValue, 801, fp);
              		   int temp2 = fgetc(fp);
              		   int temp3 = fgetc(fp);

			   printf("tempkVersion:%send\n", tempkVersion);

	    		   char tempkVersion[MAX_CMD_LEN] = {0};
		    	   strcpy(tempkVersion, keyWithVersion);

             		   check = writeMatch(sock, fp, tempkVersion, newValue, tables[z].fileCounter, txtTable); 

 		           if (check == -1) {
		               strcpy(response, "ERROR TransactionAbort\n");
        	       	       sendall(sock, response, strlen(response));
	  		       fclose(fp);
          	               return -1;
		           }

              		   if(feof(fp)) {
			
			      strcat(key, ".1.");

			      printf("writing new key to end of file. key is:%send\n", key);

	    		      int length = 51 - strlen(key);
	    		      int temp = 1;
 
    	    		      // Concatenates enough spaces to the end of the input key so that it
    	    		      // will be 50 characters long for comparison.
   	    		      while (temp < length) {
                   		     strcat(key, " ");
                   		     temp++;
            		      }	

  			      // If the end of the file has been reached and writeMatch
                              // still has not found the key, then add the new key and
                              // its corresponding value at the end of the file. 
                 	      fwrite("\n", 1, 1, fp);
                 	      fwrite(key, 1, 50, fp);
                 	      fwrite(" ", 1, 1, fp);
                              fwrite(newValue, 1, 800, fp);
                              
                              gettimeofday(&end_time, NULL);
			      int time_works = timeval_subtract (&difference, &end_time, &start_time);
			      printf ("Server processing time (set): %d.%d seconds.\n",
					difference.tv_sec, difference.tv_usec); 
                              
				totalProcessingTime.tv_sec += difference.tv_sec;
				totalProcessingTime.tv_usec += difference.tv_usec;

				printf ("Server processing time (item not in cache): %d.%d seconds.\n", 
							difference.tv_sec, difference.tv_usec); 

                              strcpy(response, "OK New: ");
                 	      strcat(response, newValue);

	    strcat(response, " ~");

	    char seconds[10];
	    itoa(totalProcessingTime.tv_sec, seconds, 10);

	    strcat(response, seconds);
	    strcat(response, ".");

	    char microsec[10];
	    itoa(totalProcessingTime.tv_usec, microsec, 10);

	    strcat(response, microsec);
 	    strcat(response, " ~");
                	      sendall(sock, response, strlen(response)); 
            	     	      sendall(sock, "\n", 1); 
                	      fclose(fp);
                 	      break;
                           } 
                    }
                return 0;
		} 
            }
	}
        // End of operation for writing to the file.


	// If the command is FIND and enough inputs are passed in to perform
        // the operation, do the following: 
	else if (fields3 == 1) {

	    printf("Beginning Query...\n");	

	    // Checks to see if file exists by reading through the table names in 
	    // the specified configuration file.
	    FILE* cf;
	    cf = fopen (config_f, "r");
	    int table_found = 0;

	    if (cf!=NULL) {

		// Checks through the entire configuration file for the table name. 
		while (!feof(cf)) {
			
			char line[MAX_CONFIG_LINE_LEN];
			char *l = fgets(line, sizeof line, cf);

			// Process the line.
			if (l == line) {

			    // Extract config parameter name and value.
			    char name[MAX_CONFIG_LINE_LEN];
			    char value[MAX_CONFIG_LINE_LEN];
			    sscanf(line, "%s %s\n", name, value);

			    // If the table name is found in the configuration file
			    // then it exists and  table_found is set to 1. 
			    if (strcmp(name, "table") == 0) {
				if (strcmp(value, justTable) == 0) {
				    table_found =1;
				    break;
				}
			    }
			}			
		}
		fclose(cf);
	    } 

	    // If the table is not in the configuration file, command cannot be 
 	    // processed. Send back the appropriate error message. 
	    if (table_found == 0) {
	        strcpy(response, "ERROR TableNotFound\n");
      	       	sendall(sock, response, strlen(response));
                return -1;
	    }

    	    // Otherwise file containing table is opened. 
            else {

    	        // Opens the file for reading.
      	        fp = fopen(table, "r");

                int cntr = 0;
                int a = 0;
	        int numberOfPredicates = 0;
                char *value = strtok(cmd, " ");   

		// Skips the first 3 inputs in the command to get to the predicate values.
		while (a < 3) {
                       strcat(skip, value);
                       
		       // Sets maxKeys (2nd value in the command received following the communication
		       // protocol agreed upon).
		       if (a == 2) {
			   maxKeys = atoi(value);
			   printf("maxKeys:%d\n", maxKeys);
		       }

                       value = strtok(NULL, " ");
                       a++;
                }

		// Parses the input predicates, setting their values in the predicate structure 
		// for easy retrieval later on.
		while (cntr < MAX_COLUMNS_PER_TABLE && value != NULL) {

		       if (cntr != 0) {
			   value = strtok(NULL, " ");
			   if (value == NULL) {
                               break;
			   }
                       }

		       strcpy(predicate[cntr].columnName, value);
		       value = strtok(NULL, " ");
		       strcpy(predicate[cntr].operation, value);
		       value = strtok(NULL, ",");
		       strcpy(predicate[cntr].value, value);
		       cntr++;
		       numberOfPredicates++;
		}

		printf("Number of predicates:%d\n", numberOfPredicates);

		int i = 0;

		// Calls the checkExistence function to make sure that all the predicates in 
		// the predicate structure is valid based on what is given in the configuration 
		// file.
		while (i < numberOfPredicates) {
		       int test = checkExistence(i, sock, txtTable);

		       if (test == 0) {
			   printf("checkExistence is zero! Invalid predicate field found!\n");
			   strcpy(response, "ERROR InvalidParameter\n");
      	       		   sendall(sock, response, strlen(response));
                	   fclose(fp);
                	   return -1; 
		       }
		       else if (test == -1) {
                           return -1;
		       }
		       else {
                           i++;
		       }

		}
	
 		char keyVal[52] = {0};
		char* fileKey = NULL;
        	char fileValue[801] = {0};
        	char sendValue[801] = {0};
        	char response [MAX_CMD_LEN];
       		int k = 0; 
        	int j = 0;
		int l = 0;
		int num = 0;
		int doesNotMatch = 0;

		int keysReturned = 0;
		int totalMatches = 0;
		char keysToReturn[MAX_CMD_LEN] = {0};
		char oneLine[MAX_CMD_LEN] = {0};
		char tempOneLine[MAX_CMD_LEN] = {0};
		int columnFinderCounter = 0;
	
		while(!feof(fp)) {
		      
		      // Reads the first 50 characters starting from the location of the 
        	      // current file pointer into the character array keyVal.
        	      while(k < 51) {
             	            keyVal[k] = fgetc(fp);
             	            k++; 
        	      }

		      k = 0;

		      // Signal that the end of the file has been reached.
             	      if (keyVal[0] == -1) {
		          break;
		      }

		      // Reached a line where a previous value has been deleted. Skip over the 
		      // next 800 empty spaces.
		      else if (strcmp(keyVal, "                                                   ") == 0) {
		          printf("Skipping over a deleted value\n");
		          fgets(fileValue, 800, fp);
		          fgetc(fp);
	                  fgetc(fp);
		      }

		      else {

			  fileKey = strtok(keyVal, ".");

			  // Reads the values field from the file
		          fgets(fileValue, 800, fp);
		          fgetc(fp);
	                  fgetc(fp);

		          num = 0;
		          columnFinderCounter = 0;
		          doesNotMatch = 0;
		          strcpy(oneLine, fileValue);

			  // For each column, check that all predicates are satisfied.
 	                  while (num < numberOfPredicates) {
		
		                 columnFinderCounter++;		
		                 strcpy(tempOneLine, oneLine);
		                 columnFinder(tempOneLine, num);

				 // Case where the operation is "="
		                 if (strcmp(predicate[num].operation, "=" ) == 0) {
			             printf("Enters =\n");
			
				     // Does the "=" comparison for floats and ints
		    		     if ((strcmp(predicate[num].type, "int") == 0 || 
					 strcmp(predicate[num].type, "float") == 0)) {
			                 printf("Equal comparison for floats and ints\n");

					 // Enters if the two fields being compared is equal.
			                 if (atof(fileSearch) == atof(predicate[num].value)) {

					     // If this is the last predicate being checked and all previous
					     // predicates have been valid, then the total number of matching
				    	     // keys is incremented.
			                     if (columnFinderCounter == numberOfPredicates && doesNotMatch == 0) {
			                         totalMatches++;
				                 
						 // Only need to return maxKeys number of keys.
						 if (keysReturned < maxKeys) { 
			    	                     strcat(keysToReturn, fileKey);
			    	                     strcat(keysToReturn, ",");
			    	                     keysReturned++;
				                 }
			                     }
		
					     // Increments num to search through the next predicate.
 			                     num++;
		                             fileSearch = NULL;
			                 }

					 // If a function ever reaches here then one of the predicates did not match.
			                 else {
	            	    		     doesNotMatch = 1;
		    	    		     num++;
		            		     fileSearch = NULL;
					 }
		    		     } 		 

		                     else {

					 // Does the "=" comparison for chars
					 // Enters if the two fields being compared is equal.
		        		 if (strcmp(predicate[num].value, fileSearch) == 0) {

					     // If this is the last predicate being checked and all previous
					     // predicates have been valid, then the total number of matching
				    	     // keys is incremented.
			    		     if (columnFinderCounter == numberOfPredicates && doesNotMatch == 0) {
			       			 totalMatches++;

						 // Only need to return maxKeys number of keys.
						 if (keysReturned < maxKeys) {
			    	    		     strcat(keysToReturn, fileKey);
			    	    		     strcat(keysToReturn, ",");
			    	    		     keysReturned++;
						 }
			    		     }

					     // Increments num to search through the next predicate.
                            		     num++;
		            		     fileSearch = NULL;
		        		 }
		
					 // If a function ever reaches here then one of the predicates did not match.
					 else {
                    	    		     doesNotMatch = 1;
		            		     num++;
		            		     fileSearch = NULL;
					 }
		    		     }
		                 }

				 // Case where the operation is ">". Only valid for floats and ints.
				 else if (strcmp(predicate[num].operation, ">" ) == 0) {
				     printf("enters >\n");

				     // Enters if the value from the file is > the predicate input.
		    		     if (atof(fileSearch) > atof(predicate[num].value)) {

					 // If this is the last predicate being checked and all previous
					 // predicates have been valid, then the total number of matching
				    	 // keys is incremented.			
			                 if (columnFinderCounter == numberOfPredicates && doesNotMatch == 0) {
  		            		     totalMatches++;

					     // Only need to return maxKeys number of keys.
			    		     if (keysReturned < maxKeys) {
			    		         strcat(keysToReturn, fileKey);
			    			 strcat(keysToReturn, ",");
			    			 keysReturned++;
			    		     }
					 }

					 // Increments num to search through the next predicate.
					 num++;
					 fileSearch = NULL;
		    		     }
	
				     // If a function ever reaches here then one of the predicates did not match.
		   		     else {
	        			 doesNotMatch = 1;
		        		 num++;
					 fileSearch = NULL;
		    		     }
				 }

				 // Case where the operation is "<". Only valid for floats and ints.
				 else if (strcmp(predicate[num].operation, "<" ) == 0) {
				     printf("enters <\n");

				     // Enters if the value from the file is < the predicate input.
		    		     if (atof(fileSearch) < atof(predicate[num].value)) {

					 // If this is the last predicate being checked and all previous
					 // predicates have been valid, then the total number of matching
				    	 // keys is incremented.
				         if (columnFinderCounter == numberOfPredicates && doesNotMatch == 0) {
			    		     totalMatches++;

					     // Only need to return maxKeys number of keys.
			    		     if (keysReturned < maxKeys) {
			    			 strcat(keysToReturn, fileKey);
			    			 strcat(keysToReturn, ",");			
			    			 keysReturned++;
			    		     }
					 }

					 // Increments num to search through the next predicate.
		        		 num++;
		        		 fileSearch = NULL;
		    		     }

				     // If a function ever reaches here then one of the predicates did not match.
		    		     else {
	                		 doesNotMatch = 1;
		        		 num++;
		        		 fileSearch = NULL;
		    		     }
		 		 }
	    		  }
	 	      }

		      // Clears variables at the end of each search.
		      for (l = 0; l < 52; l++) {		
		           keyVal[l] = 0;
		      }
		}

		// Once the entire file has been searched through, send the response back to the 
		// client side of the network following appropriate communication protocol procedures.
		char matches[MAX_CMD_LEN];
		itoa(totalMatches, matches, 10);

		strcpy(response, "Found ");
		strcat(response, matches);
		strcat(response, ":");
		strcat(response, keysToReturn);
		strcat(response, "\n");
		sendall(sock, response, strlen(response));
		printf("Length returned:%d\n",strlen(response));
		fclose(fp);

            return 0; 
    	    }
	}

        // If user command did not have fields as expected, function fails. 
	return -1;
}






///////////////////////////////////////////////////helper functions from echoselect.c//////////////////////////

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int max(int num, int * fds){
    int i;
    int r = fds[0];

    for (i = 1; i < num; i++) {
        if (fds[i] > r) 
            r = fds[i];
    }

    return r;
}

int emptySlot(int num, int * fds) {
    int i;
    for (i = 0; i < num; i++) {
        if (fds[i] < 0)
            return i;
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







/**
 * @brief Start the storage server.
 *
 * This is the main entry point for the storage server.  It reads the
 * configuration file, starts listening on a port, and proccesses
 * commands from clients.
 */
int main(int argc, char *argv[])
{
	// Process command line arguments.
	// This program expects exactly one argument: the config file name.
	assert(argc > 0);
	if (argc != 2) {
		printf("Usage %s <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char *config_file = argv[1];
	strncpy (config_f, config_file, strlen(config_file));
	

	// Read the config file.
	struct config_params params;
	int status = read_config(config_file, &params);
	if (status != 0) {
		printf("Error processing config file.\n");
		exit(EXIT_FAILURE);
	}
	LOG(("Server on %s:%d using data directory %s\n", params.server_host, params.server_port, params.data_directory));



/////////////////////////////////////////////////////////////////////////////////////////////////////
/* Initialize cache */
		int cacheSize = params.cache_size;

		printf("cache_policy:%s\n", params.cache_policy);
		printf("cache_size:%d\n", params.cache_size);
			
		if ((strcmp(params.cache_policy, "on") == 0) && (params.cache_size > 0)) {

		printf("enters cache initialization\n");
			

			int i, j, p, q = 0;
			for (i = 0; i < MAX_TABLES; i++) {
				for (j = 0; j < MAX_RECORDS_PER_TABLE; j++) {					
					for (p = 0; p < 40; p++) {					
						totalCache[i].cache[j].key[p] = 0;
					}
					for (q = 0; q < 802; q++) { 
						totalCache[i].cache[j].record[q] = 0;
					}
					totalCache[i].cache[j].timesAccessed = 0;
					totalCache[i].cache[j].timeStamp = 0;
					totalCache[i].cache[j].version = 0;
				}
				totalCache[i].numItems = 0;
				//talCache[i].tableName = {0};
			}
		
		printf("%d\n", totalNumberOfTables);
			char table[MAX_TABLE_LEN];
			int k = 0;
			FILE* fptr = NULL;

			for (k = 0; k < totalNumberOfTables; k++) {
				strcpy(table, tables[k].name);
			
				strcpy(totalCache[k].tableName, table);
		
				printf("table to be opened is:%send\n", table);

				// Sets file_path to the full directory
				char file_path[MAX_PATH_LEN + MAX_TABLE_LEN]= "\0";
				strcat (file_path, params.data_directory);
				strcat (file_path, "/");
				strcat (file_path, table);
	
				strcpy(table, file_path);

				// Opens the file for reading.
      	       			fptr = fopen(table, "r");
				
				printf("opens file?\n");

				char keyVal[51] = {0};

				char* fileKey = NULL;
				char* fileVersion = NULL;

       				char value[801] = {0};
        			char sendValue[801] = {0};
        			char response [MAX_CMD_LEN];
        			int l = 0; 
        			int m = 0;
				char o;
				int checking = 0;

				// Reads the first 50 characters starting from the location of the 
        			// current file pointer into the character array keyVal.
				for (m = 0; m < cacheSize; m++) {        			

					while(l < 50) {
             			   	   keyVal[l] = fgetc(fptr);
             			    	   l++; 
        				}
				
					if (keyVal[0] == -1) {
					    checking = m;
					    printf("check:%d\n", checking);
  					    break;
             				}

				        // Reached a line where a previous value has been deleted. Skip over the 
		      		    	// next 800 empty spaces.
 
		      		else if (strcmp(keyVal, "                                                  ") == 0) {
        				    char fileValue[801] = {0};
		          		    printf("Skipping over a deleted value\n");
					    o = fgetc(fptr);
		          		    fgets(fileValue, 800, fptr);
		          		    fgetc(fptr);
	                  		    fgetc(fptr);
					    m = m-1;
					    l = 0;
		      			} 

					else {
					    printf("key value %d is %s\n", m, keyVal);

					    fileKey = strtok(keyVal, ".");
					    fileVersion = strtok(NULL, ".");				
				
					    strcpy(totalCache[k].cache[m].key, fileKey); 
					    totalCache[k].cache[m].version = atoi(fileVersion);

					    printf("in loop: key %d is %s\n", m, totalCache[k].cache[m].key);
					    printf("in loop: version %d is %d\n", m, totalCache[k].cache[m].version);

					    o = fgetc(fptr);
            				    fgets(value, 800, fptr);

              	      			    int Temp = fgetc(fptr);
              	      			    int Temp2 = fgetc(fptr);

					    strcpy(totalCache[k].cache[m].record, value);
					    printf("record %d is %s\n", m, totalCache[k].cache[m].record);

					    totalCache[k].numItems++;

					    l = 0;
					}
 				}

				numberOfCaches++;

           			// Once the information has been sent, the file is closed and the 
 		                // function returns a 1 to indicate success.    
            			fclose(fptr);
			}
		}

		int x = 0;
		int y = 0;

		for (x = 0; x < totalNumberOfTables; x++) {
			for (y = 0; y < cacheSize; y++) {
				printf("key %d is %s\n", y, totalCache[x].cache[y].key);
				printf("record %d is %s\n", y, totalCache[x].cache[y].record);
				printf("versuib %d is %d\n", y, totalCache[x].cache[y].version);
			}
			printf("Name of Each Cache:%s\n", totalCache[x].tableName);
			printf("Total number of items in each cache:%d\n", totalCache[x].numItems);
		}



///////////////////////////////////////////////////////////////////////////////////////////////////////////Cache Initialization Completed



if (params.concurrency == 1){

/**************************************************SELECTCODE*************************************************************/

    char user_port[MAX_PORT_LEN];


    //void itoa(int value, char* str, int base) {
	
    itoa (params.server_port, &user_port, 10);


    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes2=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    int sock_fd[MAX_TO_HANDLE];
    int i, tmp, numRecv;
    char buf[BUF_SIZE];
    fd_set rfds;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP


    printf("It came here\n");

    //changed PORT to server_port
    if ((rv = getaddrinfo(NULL, user_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    printf("It came here\n");


    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes2,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    sock_fd[0] = sockfd;
    for (i = 1; i < MAX_TO_HANDLE; i++) 
        sock_fd[i] = -1;

    FD_ZERO(&rfds);
    FD_SET(sock_fd[0], &rfds);

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections on port %s...\n", user_port);

    while(1) { 
        select(max(MAX_TO_HANDLE, sock_fd) + 1, &rfds, NULL, NULL, NULL);
        if (FD_ISSET(sock_fd[0], &rfds)) {
            tmp = emptySlot(MAX_TO_HANDLE, sock_fd);
            if (tmp >= 0) {
                sock_fd[tmp] = accept(sock_fd[0], NULL, NULL);
                printf("open socket %i\n", sock_fd[tmp]);
            }
        }
        FD_SET(sock_fd[0], &rfds);

        for (i = 1; i < MAX_TO_HANDLE; i++) {
            if (sock_fd[i] < 0)
                continue;


	    ///put handle commands stuff here

            if (FD_ISSET(sock_fd[i], &rfds)) {

                //numRecv = recv(sock_fd[i], buf, BUF_SIZE-1, 0);
		char cmd[MAX_CMD_LEN];
		int status = recvline(sock_fd[i], cmd, MAX_CMD_LEN);
                //if (numRecv <= 0) {
		if (status!=0) {
                    printf("close socket %i\n", sock_fd[tmp]);
                    close(sock_fd[i]);
                    FD_CLR(sock_fd[i], &rfds);
                    sock_fd[i] = -1;
                    continue;
                }

		handle_command(sock_fd[i], cmd, &params);
                //buf[numRecv] = '\0';
                //printf("%s\n", buf);
            }
            FD_SET(sock_fd[i], &rfds);
        }
    }

    return 0;

/******************************************************************SELECT CODE ENDS HERE***************************************************************/
}////end brace for select


else if (params.concurrency == 0)
{


	// Create a socket.
	int listensock = socket(PF_INET, SOCK_STREAM, 0);
	if (listensock < 0) {
		printf("Error creating socket.\n");
		exit(EXIT_FAILURE);
	}

	// Allow listening port to be reused if defunct.
	int yes = 1;
	status = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
	if (status != 0) {
		printf("Error configuring socket.\n");
		exit(EXIT_FAILURE);
	}

	// Bind it to the listening port.
	struct sockaddr_in listenaddr;
	memset(&listenaddr, 0, sizeof listenaddr);
	listenaddr.sin_family = AF_INET;
	listenaddr.sin_port = htons(params.server_port);
	inet_pton(AF_INET, params.server_host, &(listenaddr.sin_addr)); // bind to local IP address
	status = bind(listensock, (struct sockaddr*) &listenaddr, sizeof listenaddr);
	if (status != 0) {
		printf("Error binding socket.\n");
		exit(EXIT_FAILURE);
	}

	// Listen for connections.
	status = listen(listensock, MAX_LISTENQUEUELEN);
	if (status != 0) {
		printf("Error listening on socket.\n");
		exit(EXIT_FAILURE);
	}


	// Listen loop.
	int wait_for_connections = 1;
	while (wait_for_connections) {
		// Wait for a connection.
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof clientaddr;
		int clientsock = accept(listensock, (struct sockaddr*)&clientaddr, &clientaddrlen); //blocks all other calls..we need to change stuff from here
		if (clientsock < 0) {
			printf("Error accepting a connection.\n");
			exit(EXIT_FAILURE);
		}
		LOG(("Got a connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port));

		// Get commands from client.
		int wait_for_commands = 1;
		do {
			// Read a line from the client..............critical point
			char cmd[MAX_CMD_LEN];
			int status = recvline(clientsock, cmd, MAX_CMD_LEN);
			if (status != 0) {
				// Either an error occurred or the client closed the connection.
				wait_for_commands = 0;
			} else {
				// Handle the command from the client.
				int status = handle_command(clientsock, cmd, &params);
				if (status != 0)
					wait_for_commands = 0; // Oops.  An error occured.
			}
		} while (wait_for_commands);
		
		// Close the connection with the client.
		close(clientsock);
		LOG(("Closed connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port));
	}

	// Stop listening for connections.
	close(listensock);

	return EXIT_SUCCESS;

}//end brace for non-concurrent server
else {
	printf ("Method of connection not specified. Exiting.\n");
}



}

