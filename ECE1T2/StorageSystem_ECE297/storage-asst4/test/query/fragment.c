START_TEST (testquery1BASIC)
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

	// Query for stops > 10.
	// There should be 3 matches, but ask for only one result.
	int matches = storage_query(TABLE1NAME, "stops = 5", keys_found, 1, conn);
	fail_unless(matches == 1, "Query should find 1 matching record.");
	
	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be either "first" or "third" or "fourth".
	fail_unless(
		strcmp(keys_found[0], "second") == 0, 
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
