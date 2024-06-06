/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <iostream>
#include <sql.h>
#include <sqlext.h>

void checkError(SQLRETURN ret, SQLHANDLE handle, SQLSMALLINT type, const std::string& msg) {
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR state[1024];
        SQLCHAR text[1024];
        SQLSMALLINT len;
        SQLINTEGER native;
        SQLGetDiagRec(type, handle, 1, state, &native, text, sizeof(text), &len);
        std::cerr << msg << " - " << text << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret;

    // Allocate environment handle
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    checkError(ret, env, SQL_HANDLE_ENV, "SQLAllocHandle ENV");

    // Set the ODBC version environment attribute
    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    checkError(ret, env, SQL_HANDLE_ENV, "SQLSetEnvAttr");

    // Allocate connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    checkError(ret, dbc, SQL_HANDLE_DBC, "SQLAllocHandle DBC");

    // Connect to the DSN
    ret = SQLDriverConnect(dbc, NULL, (SQLCHAR*)"DSN=FirebirdDSN;", SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    checkError(ret, dbc, SQL_HANDLE_DBC, "SQLDriverConnect");

    // // Disable auto-commit mode
    // ret = SQLSetConnectAttr(dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_IS_UINTEGER);
    // checkError(ret, dbc, SQL_HANDLE_DBC, "SQLSetConnectAttr AUTOCOMMIT_OFF");

    // Allocate statement handle
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    checkError(ret, stmt, SQL_HANDLE_STMT, "SQLAllocHandle STMT");

    // // Example: Create a table
    // ret = SQLExecDirect(stmt, (SQLCHAR*)"CREATE TABLE test (id INT PRIMARY KEY, name VARCHAR(50))", SQL_NTS);
    // checkError(ret, stmt, SQL_HANDLE_STMT, "SQLExecDirect CREATE TABLE");

    // // Example: Insert a record
    // ret = SQLExecDirect(stmt, (SQLCHAR*)"INSERT INTO test (id, name) VALUES (1, 'John Doe')", SQL_NTS);
    // checkError(ret, stmt, SQL_HANDLE_STMT, "SQLExecDirect INSERT");

    // // Commit the transaction
    // ret = SQLEndTran(SQL_HANDLE_DBC, dbc, SQL_COMMIT);
    // checkError(ret, dbc, SQL_HANDLE_DBC, "SQLEndTran SQL_COMMIT");

    // Example: Query the table
    ret = SQLExecDirect(stmt, (SQLCHAR*)"SELECT id, name FROM test_table", SQL_NTS);
    checkError(ret, stmt, SQL_HANDLE_STMT, "SQLExecDirect SELECT");

    SQLINTEGER id;
    SQLCHAR name[50];
    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(stmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
        std::cout << "ID: " << id << ", Name: " << name << std::endl;
    }

    // // Example: Update a record
    // ret = SQLExecDirect(stmt, (SQLCHAR*)"UPDATE test SET name = 'Jane Doe' WHERE id = 1", SQL_NTS);
    // checkError(ret, stmt, SQL_HANDLE_STMT, "SQLExecDirect UPDATE");

    // // Commit the transaction
    // ret = SQLEndTran(SQL_HANDLE_DBC, dbc, SQL_COMMIT);
    // checkError(ret, dbc, SQL_HANDLE_DBC, "SQLEndTran SQL_COMMIT");

    // // Example: Delete a record
    // ret = SQLExecDirect(stmt, (SQLCHAR*)"DELETE FROM test WHERE id = 1", SQL_NTS);
    // checkError(ret, stmt, SQL_HANDLE_STMT, "SQLExecDirect DELETE");

    // // Commit the transaction
    // ret = SQLEndTran(SQL_HANDLE_DBC, dbc, SQL_COMMIT);
    // checkError(ret, dbc, SQL_HANDLE_DBC, "SQLEndTran SQL_COMMIT");

    // Cleanup
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);

    return 0;
}