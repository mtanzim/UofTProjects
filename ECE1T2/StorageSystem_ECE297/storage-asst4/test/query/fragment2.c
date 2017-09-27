//####	QUERY CALL	

	//allocate memory for next query call.
	for (i = 0; i < num_keys_found; i++) {
		keys_found[i] = (char*)malloc(MAX_KEY_LEN);
		memset(keys_found[i], 0, MAX_KEY_LEN);
	}
	// Query for "name = some station".
	// There should be 1 match, but ask for only one result.
	matches = storage_query(TABLE1NAME, "name = some station", keys_found, 1, conn);
	fail_unless(matches == 1, "Query should find 1 matching record.");

	// Print the results (for debugging).
	for (i = 0; i < num_keys_found; i++) {
		printf("Query result key %d: %s\n", i, keys_found[i]);
	}

	// The matching key should be "third".
	fail_unless( 
		strcmp(keys_found[0], "third") == 0, 
		"The returned key does not match the query.\n");

	// Make sure the second key is not set to anything.
	fail_unless(keys_found[1][0] == '\0', "There should only be one key returned.\n");

	// Free memory.
	for (i = 0; i < num_keys_found; i++) {
		free(keys_found[i]);
	}
