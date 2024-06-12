/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/* clang-format off */
/**
 * The SQL statements executed in this program.
 * CREATE TABLE TEST2_TABLE (id INTEGER NOT NULL, name VARCHAR(50), PRIMARY KEY (id));
 * INSERT INTO TEST2_TABLE (id, name) VALUES (1, 'Alice');
 * INSERT INTO TEST2_TABLE (id, name) VALUES (2, 'Bob');
 * SELECT * FROM TEST2_TABLE;
 * UPDATE TEST2_TABLE SET name = 'Charlie' WHERE id = 2;
 * SELECT * FROM TEST2_TABLE;
 * DELETE FROM TEST2_TABLE WHERE id = 1;
 * SELECT * FROM TEST2_TABLE;
 * DELETE FROM TEST2_TABLE WHERE id = 2;
 * DROP table TEST2_TABLE;
*/
/* clang-format on */
// #include <algorithm>
// #include <csignal>
#include <firebird.hpp>
#include <iostream>

using namespace std::literals;

int main() {
    // Create a connection to the Firebird database
    fb::database emp("127.0.0.1:/opt/firebird/test_database.fdb", "test",
                     "test");

    // Connect to the database
    emp.connect();

    try {
        // Create a new table 'TEST2_TABLE' with columns 'id' and 'name'
        emp.execute_immediate(
            "CREATE TABLE TEST2_TABLE (id INTEGER NOT NULL, name VARCHAR(50), "
            "PRIMARY KEY (id))");
        emp.commit();  // Commit the transaction to save changes
    } catch (const std::exception& ex) {
        // Print any errors that occur during table creation
        std::cout << "ERROR: " << ex.what() << std::endl;
        return -1;
    }

    try {
        // Insert records into the 'TEST2_TABLE'
        emp.execute_immediate(
            "INSERT INTO TEST2_TABLE (id, name) VALUES (1, 'Alice')");
        emp.execute_immediate(
            "INSERT INTO TEST2_TABLE (id, name) VALUES (2, 'Bob')");
        emp.commit();  // Commit the transaction to save changes
    } catch (const std::exception& ex) {
        // Print any errors that occur during record insertion
        std::cout << "ERROR: " << ex.what() << std::endl;
        return -1;
    }

    try {
        // Create a query to select all records from 'TEST2_TABLE'
        fb::query query(emp, "SELECT * FROM TEST2_TABLE");

        // Print the records retrieved by the query
        std::cout << "-------------------" << std::endl;
        for (auto& row : query.execute()) {
            std::cout << row[0].value<int>() << "  "
                      << row[1].value<std::string_view>()
                      // row[1].sql_datatype()
                      << std::endl;
        }
    } catch (const std::exception& ex) {
        // Print any errors that occur during record retrieval
        std::cout << "ERROR: " << ex.what() << std::endl;
        return -1;
    }

    try {
        // Update the 'name' field of the record with 'id' 2
        emp.execute_immediate(
            "UPDATE TEST2_TABLE SET name = 'Charlie' WHERE id = 2");
        emp.commit();  // Commit the transaction to save changes
    } catch (const std::exception& ex) {
        // Print any errors that occur during the update
        std::cout << "ERROR: " << ex.what() << std::endl;
        return -1;
    }

    try {
        // Create a query to select all records from 'TEST2_TABLE'
        fb::query query(emp, "SELECT * FROM TEST2_TABLE");

        // Print the records retrieved by the query after the update
        std::cout << "-------------------" << std::endl;
        for (auto& row : query.execute()) {
            std::cout << row[0].value<int>() << "  "
                      << row[1].value<std::string_view>() << std::endl;
        }
    } catch (const std::exception& ex) {
        // Print any errors that occur during record retrieval after the update
        std::cout << "ERROR: " << ex.what() << std::endl;
        return -1;
    }

    try {
        // Delete the record with 'id' 1 from 'TEST2_TABLE'
        emp.execute_immediate("DELETE FROM TEST2_TABLE WHERE id = 1");
        emp.commit();  // Commit the transaction to save changes
    } catch (const std::exception& ex) {
        // Print any errors that occur during the deletion
        std::cout << "ERROR: " << ex.what() << std::endl;
        return -1;
    }

    try {
        // Create a query to select all records from 'TEST2_TABLE'
        fb::query query(emp, "SELECT * FROM TEST2_TABLE");

        // Print the records retrieved by the query after the deletion
        std::cout << "-------------------" << std::endl;
        for (auto& row : query.execute()) {
            std::cout << row[0].value<int>() << "  "
                      << row[1].value<std::string_view>() << std::endl;
        }
    } catch (const std::exception& ex) {
        // Print any errors that occur during record retrieval after the
        // deletion
        std::cout << "ERROR: " << ex.what() << std::endl;
        return -1;
    }

    try {
        // Delete the record with 'id' 2 from 'TEST2_TABLE'
        emp.execute_immediate("DELETE FROM TEST2_TABLE WHERE id = 2");
        emp.commit();  // Commit the transaction to save changes

        emp.execute_immediate("DROP table TEST2_TABLE");
        emp.commit();  // Commit the transaction to save changes

    } catch (const std::exception& ex) {
        // Print any errors that occur during the deletion
        std::cout << "ERROR: " << ex.what() << std::endl;
        return -1;
    }

    try {
        // Disconnect from the database
        emp.disconnect();
    } catch (const std::exception& ex) {
        // Print any errors that occur during disconnection
        std::cout << "ERROR: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
