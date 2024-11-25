/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <simdjson.h>
#include <iostream>

int main()
{
    // JSON string
    const char *json = R"({
        "name": "Alice",
        "age": 30,
        "city": "Wonderland"
    })";

    // Create a simdjson::dom::parser object
    simdjson::dom::parser parser;

    // Parse the JSON string
    simdjson::dom::element doc;
    auto error = parser.parse(json, std::strlen(json)).get(doc);
    if (error) {
        std::cerr << "Parsing error: " << error << std::endl;
        return 1;
    }

    // Extract field values
    std::string_view name;
    int64_t age;
    std::string_view city;

    if (doc["name"].get(name) || doc["age"].get(age) || doc["city"].get(city)) {
        std::cerr << "Error extracting field values" << std::endl;
        return 1;
    }

    // Output the results
    std::cout << "Name: " << name << std::endl;
    std::cout << "Age: " << age << std::endl;
    std::cout << "City: " << city << std::endl;

    return 0;
}
