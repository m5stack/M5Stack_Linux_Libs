#ifndef MAILBOX_H
#define MAILBOX_H
#include "easy_msg.h"
#include "easymsg_subscriber.h"
namespace test {
// 项目中需要传递的结构体
struct SomeMsg {
  std::string msg_str;
};

struct SomeMsg2 {
  std::string msg_str2;
};

// 将其注册为消息
EASY_MSG_DEFINE(SomeMsg, SomeMsgId);
EASY_MSG_DEFINE(SomeMsg2, SomeMsg2Id);

class MailBox {
public:
  MailBox();

  void handleMsg(em::EasyMsg *easymsg);

private:
  em::EasyMsgSubscriber sub; // 析构时自动取消订阅
};

} // namespace test
#endif // MAILBOX_H
