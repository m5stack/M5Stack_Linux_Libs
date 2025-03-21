/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <gdbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Open or create a GDBM database
    GDBM_FILE db;
    const char *db_name = "example.db";
    db = gdbm_open(db_name, 0, GDBM_WRCREAT, 0666, NULL);
    if (!db) {
        fprintf(stderr, "Failed to open the database: %s\n", db_name);
        return 1;
    }

    // Insert a key-value pair into the database
    datum key, value;

    key.dptr = "name";
    key.dsize = strlen(key.dptr) + 1;
    value.dptr = "Alice";
    value.dsize = strlen(value.dptr) + 1;

    if (gdbm_store(db, key, value, GDBM_REPLACE) != 0) {
        fprintf(stderr, "Failed to insert data\n");
        gdbm_close(db);
        return 1;
    }

    printf("Data inserted successfully: key = %s, value = %s\n", key.dptr, value.dptr);

    // Retrieve the value associated with the key
    datum result = gdbm_fetch(db, key);
    if (result.dptr) {
        printf("Data retrieved successfully: key = %s, value = %s\n", key.dptr, result.dptr);
        free(result.dptr); // Free the memory allocated by gdbm_fetch
    } else {
        printf("Failed to retrieve data: key = %s\n", key.dptr);
    }

    // Delete the key-value pair
    if (gdbm_delete(db, key) == 0) {
        printf("Data deleted successfully: key = %s\n", key.dptr);
    } else {
        printf("Failed to delete data: key = %s\n", key.dptr);
    }

    // Close the database
    gdbm_close(db);
    printf("Database closed\n");

    return 0;
}
