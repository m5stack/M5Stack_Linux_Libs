# 设计目标
 - 提供C++对象进程内通信功能 可进行消息传递；
 - 将已经存在的结构体定义为消息时，不能破坏已经存在的结构体本身的结构；
 - 处理消息的类无需继承任何基类；
 - 足够简单的订阅方法；
 - RAII形式的取消订阅，但也支持手动取消订阅。

# 使用导航

## 最小示例
```cpp
#include "easy_msg.h"
#include "easymsg_subscriber.h"
#include <iostream>
//项目中需要传递的结构体
struct SomeMsg {
  std::string msg_str;
};

// 将其注册为消息
EASY_MSG_DEFINE(SomeMsg, SomeMsgId);

class MailBox {
public:
  MailBox() {
    //注册消息回调函数
    sub.subscribe<SomeMsgId>(this, &MailBox::handleMsg);
  }

  void handleMsg(em::EasyMsg *easymsg) {
    //使用 msg函数
    if (easymsg->match<SomeMsgId>()) {
      auto *p = em::easymsg_cast<SomeMsgId>(easymsg);
      std::cout << p->msg_str << std::endl;
    }
  }

private:
  em::EasyMsgSubscriber sub; //析构时自动取消订阅
};

int main() {
  //1 先创建原始结构体 然后进行赋值
  SomeMsg msg1{"shshhshsh"};
  // 填充消息字段 可以用原始结构体构造
  SomeMsgId::MsgType msg{msg1};
  // 当然 也可以重新为字段赋值
  // msg.msg_str = "ada";

  //2 直接进行赋值
  SomeMsg2Id::MsgType msg2;
  msg2.msg_str2 = "blabalal";

  //3 同1 可以使用move
  SomeMsg msg3{"msg 3 move test"};
  SomeMsgId::MsgType msg_move_test{std::move(msg3)};

  MailBox box;
  // 消息分发 执行回调
  em::sendMsg<SomeMsgId>(&msg);
  em::sendMsg<SomeMsg2Id>(&msg2);
  em::sendMsg<SomeMsgId>(&msg_move_test);
  return 0;
}

```


## 消息和消息ID

假设你有一个原始结构体 
```cpp
struct SomeMsg {
  std::string msg_str;
};
```
定义消息将会非常简单

```cpp
EASY_MSG_DEFINE(SomeMsg, SomeMsgId);
```
EASY_MSG_DEFINE 的第一个参数为原始消息结构体，这个结构体定义了消息内容，第二个参数是由用户决定的名称，这个名称将作为消息ID使用，消息ID再订阅消息，发送消息和处理消息时十分重要。

## 消息订阅
如果某对象对指定消息感兴趣，则可以订阅它，订阅消息的宿主对象无需进行额外的继承，正如`MailBox`类那样：

```cpp
class MailBox {
public:
  MailBox() {
    //注册消息回调函数
    sub.subscribe<SomeMsgId>(this, &MailBox::handleMsg);
  }

  void handleMsg(em::EasyMsg *easymsg) {
    //使用 msg函数
    if (easymsg->match<SomeMsgId>()) {
      auto *p = em::easymsg_cast<SomeMsgId>(easymsg);
      std::cout << p->msg_str << std::endl;
    }
  }

private:
  em::EasyMsgSubscriber sub; //析构时自动取消订阅
};
```
`EasyMsgSubscriber`是EasyMsgCpp提供的订阅辅助工具，用来帮助订阅者自动进行取消订阅。订阅函数是一个模板函数，模板参数为宿主关心的消息id， 消息id是消息定义时由用户指定的。第一个参数是宿主对象，第二个参数是宿主对象的一个成员函数。
消息的发送方调用sendMsg发送指定类型的消息对象时，对应的回调函数将会被调用。

## 匹配和处理消息
```cpp
  void handleMsg(em::EasyMsg *easymsg) {
    if (easymsg->match<SomeMsgId>()) {
      auto *p = em::easymsg_cast<SomeMsgId>(easymsg);
      std::cout << p->msg_str << std::endl;
    }
  }
```
消息回调函数应是一个参数为 `em::EasyMsg *` 类型，返回值为 `void` 类型的函数，`em::EasyMsg` 提供了 `match` 函数用来判定接受的消息类型，`em::easymsg_cast`则可以对消息进行转换，将其转化为更具体的消息类型。

