/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <bson/bson.h>
#include <stdio.h>

int main() {
    // 创建一个 bson 对象
    bson_t *document;
    document = bson_new();

    // 添加数据到 BSON 文档
    BSON_APPEND_UTF8(document, "name", "Alice");
    BSON_APPEND_INT32(document, "age", 25);
    BSON_APPEND_BOOL(document, "is_student", true);
    BSON_APPEND_DOUBLE(document, "grade", 90.5);

    // 嵌套文档（子文档）
    bson_t child;
    bson_append_document_begin(document, "address", -1, &child);
    BSON_APPEND_UTF8(&child, "city", "New York");
    BSON_APPEND_UTF8(&child, "zipcode", "10001");
    bson_append_document_end(document, &child);

    // 打印 BSON 文档为 JSON 格式
    char *json_str = bson_as_canonical_extended_json(document, NULL);
    printf("BSON as JSON:\n%s\n", json_str);

    // 解析 BSON 数据
    bson_iter_t iter;
    if (bson_iter_init(&iter, document)) {
        while (bson_iter_next(&iter)) {
            const char *key = bson_iter_key(&iter);
            bson_type_t type = bson_iter_type(&iter);

            printf("Key: %s, Type: %d\n", key, type);

            if (type == BSON_TYPE_UTF8) {
                printf("Value: %s\n", bson_iter_utf8(&iter, NULL));
            } else if (type == BSON_TYPE_INT32) {
                printf("Value: %d\n", bson_iter_int32(&iter));
            } else if (type == BSON_TYPE_BOOL) {
                printf("Value: %s\n", bson_iter_bool(&iter) ? "true" : "false");
            } else if (type == BSON_TYPE_DOUBLE) {
                printf("Value: %f\n", bson_iter_double(&iter));
            } else if (type == BSON_TYPE_DOCUMENT) {
                printf("Value: Subdocument\n");
            }
        }
    }

    // 释放资源
    bson_free(json_str);
    bson_destroy(document);

    return 0;
}
