/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <stdio.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // 初始化 MongoDB 客户端库
    mongoc_init();

    // MongoDB 连接 URI
    const char *uri_string = "mongodb://localhost:27017"; // 替换为您的 MongoDB 地址
    mongoc_uri_t *uri;
    mongoc_client_t *client;
    bson_error_t error;
    mongoc_database_t *database;
    mongoc_collection_t *collection;
    bson_t *command, reply, *insert;
    char *str;
    bool retval;

    // 创建 URI 对象
    uri = mongoc_uri_new_with_error(uri_string, &error);
    if (!uri) {
        fprintf(stderr, "Failed to parse URI: %s\nError message: %s\n", uri_string, error.message);
        return EXIT_FAILURE;
    }

    // 创建客户端
    client = mongoc_client_new_from_uri(uri);
    if (!client) {
        fprintf(stderr, "Failed to create a client!\n");
        return EXIT_FAILURE;
    }

    // 选择数据库和集合
    database = mongoc_client_get_database(client, "testdb"); // 替换为您的数据库名称
    collection = mongoc_client_get_collection(client, "testdb", "testcollection"); // 替换为您的集合名称

    // 插入文档
    insert = BCON_NEW("name", BCON_UTF8("Alice"), "age", BCON_INT32(25));
    if (!mongoc_collection_insert_one(collection, insert, NULL, NULL, &error)) {
        fprintf(stderr, "Insert failed: %s\n", error.message);
    } else {
        printf("Successfully inserted document.\n");
    }
    bson_destroy(insert);

    // 执行命令 { ping: 1 }
    command = BCON_NEW("ping", BCON_INT32(1));
    retval = mongoc_client_command_simple(client, "admin", command, NULL, &reply, &error);

    if (!retval) {
        fprintf(stderr, "Command failed: %s\n", error.message);
    } else {
        str = bson_as_canonical_extended_json(&reply, NULL);
        printf("%s\n", str);
        bson_free(str);
    }

    // 清理资源
    bson_destroy(&reply);
    bson_destroy(command);
    mongoc_collection_destroy(collection);
    mongoc_database_destroy(database);
    mongoc_uri_destroy(uri);
    mongoc_client_destroy(client);

    // 关闭 MongoDB 客户端库
    mongoc_cleanup();

    return EXIT_SUCCESS;
}
