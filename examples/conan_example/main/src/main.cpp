/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <iostream>
#include <string>
#include <json/json.h>
#include <fmt/core.h>
#include <fmt/format.h>

using namespace std;

void writeJsonExample()
{
    Json::Value root;

    // 创建JSON对象
    root["name"] = "张三";
    root["age"]  = 25;
    root["city"] = "北京";

    // 创建数组
    Json::Value skills;
    skills.append("C++");
    skills.append("Python");
    skills.append("JavaScript");
    root["skills"] = skills;

    // 创建嵌套对象
    Json::Value address;
    address["street"] = "长安街";
    address["number"] = 100;
    root["address"]   = address;

    // 输出格式化的JSON
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "  ";
    string jsonString     = Json::writeString(writer, root);

    cout << "=== 生成的JSON ===" << endl;
    cout << jsonString << endl;
}

void readJsonExample()
{
    // 模拟从字符串读取JSON
    string jsonStr = R"({
        "name": "李四",
        "age": 30,
        "city": "上海",
        "skills": ["Java", "Go", "Rust"],
        "address": {
            "street": "南京路",
            "number": 200
        }
    })";

    Json::CharReaderBuilder reader;
    Json::Value root;
    string errors;

    istringstream iss(jsonStr);
    bool parsingSuccessful = Json::parseFromStream(reader, iss, &root, &errors);

    if (!parsingSuccessful) {
        cout << "解析JSON失败: " << errors << endl;
        return;
    }

    cout << "\n=== 解析JSON ===" << endl;

    std::string name = "Alice";
    int age          = 25;

    std::string result = fmt::format("Hello, {}! You are {} years old.", name, age);
    fmt::print("{}\n", result);

    fmt::print("Hello, {}! You are {} years old.\n", name, age);

    // cout << "姓名: " << root["name"].asString() << endl;
    cout << "年龄: " << root["age"].asInt() << endl;
    cout << "城市: " << root["city"].asString() << endl;

    cout << "技能: ";
    const Json::Value& skills = root["skills"];
    for (unsigned int i = 0; i < skills.size(); i++) {
        cout << skills[i].asString();
        if (i < skills.size() - 1) cout << ", ";
    }
    cout << endl;

    cout << "地址: " << root["address"]["street"].asString() << " " << root["address"]["number"].asInt() << "号"
         << endl;
}

void modifyJsonExample()
{
    Json::Value root;
    root["version"] = "1.0.0";
    root["debug"]   = true;
    root["count"]   = 0;

    cout << "\n=== 修改JSON ===" << endl;
    cout << "原始值: " << root["count"].asInt() << endl;

    // 修改值
    root["count"] = root["count"].asInt() + 10;
    cout << "修改后: " << root["count"].asInt() << endl;

    // 删除键
    root.removeMember("debug");
    cout << "删除debug键后，是否存在: " << root.isMember("debug") << endl;
}

int main()
{
    cout << "JsonCpp 示例程序" << endl;
    cout << "=================" << endl;

    try {
        writeJsonExample();
        readJsonExample();
        modifyJsonExample();

        cout << "\n程序执行成功！" << endl;
        return 0;
    } catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        return 1;
    }
}