/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
/**
 * The SQL statements executed in this program.
 * CREATE TABLE TEST1_TABLE (id INTEGER NOT NULL, name VARCHAR(50), PRIMARY KEY (id));
 * INSERT INTO TEST1_TABLE (id, name) VALUES (1, 'Alice');
 * INSERT INTO TEST1_TABLE (id, name) VALUES (2, 'Bob');
 * SELECT * FROM TEST1_TABLE;
 * UPDATE TEST1_TABLE SET name = 'Charlie' WHERE id = 2;
 * SELECT * FROM TEST1_TABLE;
 * DELETE FROM TEST1_TABLE WHERE id = 1;
 * SELECT * FROM TEST1_TABLE;
 * DELETE FROM TEST1_TABLE WHERE id = 2;
*/
#include <ibase.h>
#include <cstring>
#include <iostream>
#include <string>

// Firebird API error handling function
void check_isc_status(ISC_STATUS *status) {
    if (status[0] == 1 && status[1]) {
        std::cerr << "Firebird error code: " << status[1] << std::endl;
        isc_print_status(status);
        // char err_msg[512];
        // fb_interpret(err_msg, sizeof(err_msg), &status);
        // std::cerr << "Firebird error message: " << err_msg << std::endl;
        exit(1);
    }
}

void execute_immediate(isc_db_handle &db, isc_tr_handle &trans,
                       const char *sql) {
    ISC_STATUS status[20];
    if (isc_dsql_execute_immediate(status, &db, &trans, 0, sql, SQL_DIALECT_V6,
                                   NULL)) {
        check_isc_status(status);
    }
}

void query_table(isc_db_handle &db, isc_tr_handle &trans, const char *sql) {
    ISC_STATUS status[20];
    isc_stmt_handle stmt = 0;  // NULL
    XSQLDA *sqlda        = (XSQLDA *)malloc(XSQLDA_LENGTH(1));
    sqlda->version       = SQLDA_VERSION1;
    sqlda->sqln          = 1;
    sqlda->sqld          = 0;

    if (isc_dsql_allocate_statement(status, &db, &stmt)) {
        check_isc_status(status);
    }

    if (isc_dsql_prepare(status, &trans, &stmt, 0, sql, SQL_DIALECT_V6,
                         sqlda)) {
        check_isc_status(status);
    }

    if (sqlda->sqld > sqlda->sqln) {
        sqlda       = (XSQLDA *)realloc(sqlda, XSQLDA_LENGTH(sqlda->sqld));
        sqlda->sqln = sqlda->sqld;
        if (isc_dsql_describe(status, &stmt, SQL_DIALECT_V6, sqlda)) {
            check_isc_status(status);
        }
    }

    for (int i = 0; i < sqlda->sqld; i++) {
        sqlda->sqlvar[i].sqldata = (char *)malloc(sqlda->sqlvar[i].sqllen);
        sqlda->sqlvar[i].sqlind  = (short *)malloc(sizeof(short));
    }

    if (isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        check_isc_status(status);
    }

    while (isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda) == 0) {
        for (int i = 0; i < sqlda->sqld; i++) {
            // XSQLVAR &var = sqlda->sqlvar[i];
            if (sqlda->sqlvar[i].sqltype == 449) {
                if (*sqlda->sqlvar[i].sqlind < 0) {
                    std::cout << "NULL ";
                } else {
                    std::cout << std::string(sqlda->sqlvar[i].sqldata,
                                             sqlda->sqlvar[i].sqllen)
                              << " ";
                }
            } else if (sqlda->sqlvar[i].sqltype == SQL_LONG) {
                    ISC_LONG data;
                    memcpy(&data, sqlda->sqlvar[i].sqldata, sizeof(ISC_LONG));
                    std::cout << data << " ";
            } else {
                std::cout << "sqltype: " << sqlda->sqlvar[i].sqltype;
            }
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < sqlda->sqld; i++) {
        free(sqlda->sqlvar[i].sqldata);
        free(sqlda->sqlvar[i].sqlind);
    }
    free(sqlda);

    if (isc_dsql_free_statement(status, &stmt, DSQL_drop)) {
        check_isc_status(status);
    }
}
int buildDpb(const char *user, const char *password, char *dpb_buffer) {
    char *dpb = dpb_buffer;

    *dpb++ = isc_dpb_version1;
    *dpb++ = isc_dpb_user_name;
    *dpb++ = strlen(user);
    strcpy(dpb, user);
    dpb += strlen(user);
    *dpb++ = isc_dpb_password;
    *dpb++ = strlen(password);
    strcpy(dpb, password);
    dpb += strlen(password);
    return dpb - dpb_buffer;
}

int main() {
    ISC_STATUS status[20];
    isc_db_handle db     = 0;
    isc_tr_handle trans  = 0;
    const char *db_name  = "127.0.0.1:/opt/firebird/test_database.fdb";
    const char *user     = "test";
    const char *password = "test";
    char dpb_buffer[256];
    int dpb_buffer_len = buildDpb(user, password, dpb_buffer);

    if (isc_attach_database(status, 0, db_name, &db, dpb_buffer_len,
                            dpb_buffer)) {
        check_isc_status(status);
    }

    if (isc_start_transaction(status, &trans, 1, &db, 0, NULL)) {
        check_isc_status(status);
    }

    // Create table
    execute_immediate(
        db, trans,
        "EXECUTE BLOCK AS BEGIN IF (NOT EXISTS (SELECT 1 FROM rdb$relations "
        "WHERE rdb$relation_name = 'TEST1_TABLE')) THEN BEGIN EXECUTE "
        "STATEMENT 'CREATE TABLE TEST1_TABLE (id INTEGER NOT NULL, name "
        "VARCHAR(50), PRIMARY KEY (id))'; END END");

    if (isc_commit_transaction(status, &trans) ||
        isc_start_transaction(status, &trans, 1, &db, 0, NULL)) {
        check_isc_status(status);
    }

    // Insert data

    execute_immediate(db, trans,
                      "INSERT INTO TEST1_TABLE (id, name) VALUES (1, 'Alice')");
    execute_immediate(db, trans,
                      "INSERT INTO TEST1_TABLE (id, name) VALUES (2, 'Bob')");
    std::cout << "Insert data:" << std::endl;
    
    // Query data
    query_table(db, trans, "SELECT * FROM TEST1_TABLE");

    // Update data
    execute_immediate(db, trans,
                      "UPDATE TEST1_TABLE SET name = 'Charlie' WHERE id = 2");
    // Query data
    std::cout << "Table content after insert and update:" << std::endl;
    query_table(db, trans, "SELECT * FROM TEST1_TABLE");

    // Delete data
    execute_immediate(db, trans, "DELETE FROM TEST1_TABLE WHERE id = 1");
    // Query data
    std::cout << "Table content after delete:" << std::endl;
    query_table(db, trans, "SELECT * FROM TEST1_TABLE");

    execute_immediate(db, trans, "DELETE FROM TEST1_TABLE WHERE id = 2");

    // Commit transaction
    if (isc_commit_transaction(status, &trans)) {
        check_isc_status(status);
    }

    // Disconnect database connection
    if (isc_detach_database(status, &db)) {
        check_isc_status(status);
    }

    return 0;
}
