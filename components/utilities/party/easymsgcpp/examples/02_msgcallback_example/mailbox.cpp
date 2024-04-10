#include "mailbox.h"
namespace test {
MailBox::MailBox() {
  //注册消息回调函数
  sub.subscribe<SomeMsgId>(this, &MailBox::handleMsg);
}

void MailBox::handleMsg(em::EasyMsg *easymsg) {
  //使用 msg函数
  if (easymsg->match<SomeMsgId>()) {
    auto *p = em::easymsg_cast<SomeMsgId>(easymsg);
    std::cout << p->msg_str << std::endl;
  }

  if (easymsg->match<SomeMsg2Id>()) {
    auto *p = em::easymsg_cast<SomeMsg2Id>(easymsg);
    std::cout << p->msg_str2 << std::endl;
  }
}
} // namespace test
