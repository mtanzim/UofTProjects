START_TEST (testINVALIDINPUTSo2o1tb)/*INVALID inputs wrt spaces and commas. All must produce INVALID returns*/
{
	// Start the server.
	int serverpid = start_server(FOURTABLES_CONF); 
		fail_if(serverpid <= 0,"Couldn't start server.");

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, SERVERPORT);
	fail_if(conn == NULL, "Couldn't connect to server.");

	// Set a key/value.
	struct storage_record r;
	strncpy(r.value, INVALUEsubway_000_0000, sizeof r.value);
	int status = storage_set(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should have error setting a key/value pair.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE1NAME, KEY, &r, conn);
	fail_if(status == 0, "Should have error retrieving invalid value.");
	fail_if(strcmp(r.value, INVALUEsubway_000_0000) == 0, "Value on disk should not match VALID input.");

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
