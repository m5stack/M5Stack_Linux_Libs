/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

// Open the database
int open_db(sqlite3 **db) {
    int rc = sqlite3_open("example.db", db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
    return rc;
}

// Create table
int create_table(sqlite3 *db) {
    char *sql =
        "CREATE TABLE IF NOT EXISTS COMPANY("
        "ID INT PRIMARY KEY     NOT NULL,"
        "NAME           TEXT    NOT NULL,"
        "AGE            INT     NOT NULL,"
        "ADDRESS        CHAR(50),"
        "SALARY         REAL );";

    char *errMsg = 0;
    int rc       = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
    return rc;
}

// Insert data
int insert_data(sqlite3 *db) {
    char *sql =
        "INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY) "
        "VALUES (1, 'Paul', 32, 'California', 20000.00 ); "
        "INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY) "
        "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "
        "INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY)"
        "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );"
        "INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY)"
        "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

    char *errMsg = 0;
    int rc       = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        fprintf(stdout, "Records created successfully\n");
    }
    return rc;
}

// Select data
int select_data(sqlite3 *db) {
    char *sql = "SELECT * from COMPANY";
    sqlite3_stmt *stmt;
    const char *pzTest;

    int rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, &pzTest);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("ID: %d, NAME: %s, AGE: %d, ADDRESS: %s, SALARY: %f\n", sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1), sqlite3_column_int(stmt, 2), sqlite3_column_text(stmt, 3),
               sqlite3_column_double(stmt, 4));
    }

    sqlite3_finalize(stmt);
    return rc;
}

// Update data
int update_data(sqlite3 *db) {
    char *sql =
        "UPDATE COMPANY set SALARY = 25000.00 where ID=1; "
        "SELECT * from COMPANY";

    char *errMsg = 0;
    int rc       = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }
    return rc;
}

// Delete data
int delete_data(sqlite3 *db) {
    char *sql =
        "DELETE from COMPANY where ID=2; "
        "SELECT * from COMPANY";

    char *errMsg = 0;
    int rc       = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }
    return rc;
}

int main(int argc, char *argv[]) {
    sqlite3 *db;
    int rc;

    rc = open_db(&db);
    if (rc) return rc;

    rc = create_table(db);
    if (rc) return rc;

    rc = insert_data(db);
    if (rc) return rc;

    rc = select_data(db);
    if (rc) return rc;

    rc = update_data(db);
    if (rc) return rc;

    rc = delete_data(db);
    if (rc) return rc;

    sqlite3_close(db);
    return 0;
}


