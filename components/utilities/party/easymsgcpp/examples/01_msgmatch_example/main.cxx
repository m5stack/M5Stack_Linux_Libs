#include "easy_msg.h"
#include <iostream>
//项目中需要传递的结构体
struct SomeMsg {
  std::string msg_str;
};

struct SomeMsg2 {
  std::string msg_str2;
};

// 将其注册为消息
EASY_MSG_DEFINE(SomeMsg, SomeMsgId);

int main() {
  // 填充消息字段
  SomeMsgId::MsgType msg;
  msg.msg_str = "ada";

  //.....收到消息后....

  // 进行消息匹配测试
  if (msg.match<SomeMsgId>()) {
    std::cout << "msg matched = " << msg.id() << std::endl;
  }
  // 消息匹配失败不会导致程序停止或编译失败
  if (msg.match<SomeMsg2>()) {
    std::cout << "msg matched = " << msg.id() << std::endl;
  }

  return 0;
}
