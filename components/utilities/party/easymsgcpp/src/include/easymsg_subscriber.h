#ifndef EASYMSGSUBSCRIBER_H
#define EASYMSGSUBSCRIBER_H

#include "easymsg_dispatcher.h"
#include "easymsg_export.h"
#include <functional>

namespace em {
class EasyMsg;
class EASYMSG_API EasyMsgSubscriber {
public:
  EasyMsgSubscriber();
  ~EasyMsgSubscriber();

  template <typename EASY_MSG_ID, typename HOST_OBJ>
  void subscribe(HOST_OBJ *hostObj, void (HOST_OBJ::*handleMsg)(EasyMsg *)) {
    EasyMsgDispatcher &dispatcher = getDispatcher();
    dispatcher.addCallback<EASY_MSG_ID>(hostObj, handleMsg);
    host_ptr = static_cast<void *>(hostObj);
  }

  template <typename EASY_MSG_ID, typename HOST_OBJ>
  void unSubscribe(HOST_OBJ *hostObj) {
    if (host_ptr != static_cast<void *>(hostObj)) {
      return;
    }
    EasyMsgDispatcher &dispatcher = getDispatcher();
    dispatcher.removeCallback<EASY_MSG_ID>(hostObj);
  }

private:
  static EasyMsgDispatcher &getDispatcher();

  void *host_ptr{};
};

} // namespace em

#endif // EASYMSGSUBSCRIBER_H
