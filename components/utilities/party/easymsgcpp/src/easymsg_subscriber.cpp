#include "easymsg_subscriber.h"
#include "easy_msg.h"
namespace em {

EasyMsgSubscriber::EasyMsgSubscriber() = default;

EasyMsgSubscriber::~EasyMsgSubscriber() {
  getDispatcher().removeCallback(host_ptr);
}

EasyMsgDispatcher &EasyMsgSubscriber::getDispatcher() { return ___dispatcher; }

} // namespace em
