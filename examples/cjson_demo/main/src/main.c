/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int main() {
    // Create a JSON object
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "John Doe");
    cJSON_AddNumberToObject(root, "age", 30);
    cJSON_AddBoolToObject(root, "employed", 1);

    // Create an array and add it to the JSON object
    cJSON *skills = cJSON_CreateArray();
    cJSON_AddItemToArray(skills, cJSON_CreateString("C"));
    cJSON_AddItemToArray(skills, cJSON_CreateString("C++"));
    cJSON_AddItemToArray(skills, cJSON_CreateString("Python"));
    cJSON_AddItemToObject(root, "skills", skills);

    // Convert the JSON object to a string and print it
    char *json_string = cJSON_Print(root);
    printf("Generated JSON:\n%s\n", json_string);

    // Parse the JSON string
    cJSON *parsed_root = cJSON_Parse(json_string);
    if (parsed_root == NULL) {
        fprintf(stderr, "Error parsing JSON string.\n");
    } else {
        // Get data from the parsed JSON object
        cJSON *name = cJSON_GetObjectItem(parsed_root, "name");
        cJSON *age = cJSON_GetObjectItem(parsed_root, "age");
        cJSON *employed = cJSON_GetObjectItem(parsed_root, "employed");
        cJSON *parsed_skills = cJSON_GetObjectItem(parsed_root, "skills");

        // Print the parsed data
        printf("Parsed JSON:\n");
        printf("Name: %s\n", name->valuestring);
        printf("Age: %d\n", age->valueint);
        printf("Employed: %s\n", employed->valueint ? "true" : "false");

        printf("Skills:\n");
        int skill_count = cJSON_GetArraySize(parsed_skills);
        for (int i = 0; i < skill_count; i++) {
            cJSON *skill = cJSON_GetArrayItem(parsed_skills, i);
            printf("  - %s\n", skill->valuestring);
        }
    }

    // Free memory
    cJSON_Delete(root);
    cJSON_Delete(parsed_root);
    free(json_string);

    return 0;
}
