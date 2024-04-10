#ifndef EASYMSGDISPATCHER_H
#define EASYMSGDISPATCHER_H

#include "easymsg_callback.h"
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
namespace em {

class EASYMSG_API EasyMsgDispatcher {
  using MsgBindTupe = std::tuple<std::int64_t, std::string, EasyMsgCallback>;
  enum MsgBindTupeIndex {
    kHostObj,
    kMsgId,
    kMsgCallback,
  };

public:
  EasyMsgDispatcher();
  //添加回调函数
  template <typename EASY_MSG_ID, typename HOST_OBJ>
  void addCallback(HOST_OBJ *host_obj,
                   void (HOST_OBJ::*handle_msg_callback)(em::EasyMsg *)) {

    std::function<void(em::EasyMsg *)> host_callback =
        std::bind(handle_msg_callback, host_obj, std::placeholders::_1);

    auto iter = find_callback(host_obj, EASY_MSG_ID::value);
    if (iter == msg_callback_vector_.end()) {
      msg_callback_vector_.emplace_back(std::make_tuple(
          reinterpret_cast<std::int64_t>(host_obj), EASY_MSG_ID::value,
          EasyMsgCallback(std::move(host_callback))));
      return;
    }
    std::get<2>(*iter) = std::move(host_callback);
  }
  //移除此 host_obj，订阅的消息为 EASY_MSG_ID 的回调函数
  template <typename EASY_MSG_ID, typename HOST_OBJ>
  void removeCallback(HOST_OBJ *host_obj) {
    auto iter = find_callback(host_obj, EASY_MSG_ID::value);
    if (iter == msg_callback_vector_.end()) {
      return;
    }
    msg_callback_vector_.erase(iter);
 }
  //移除此 host_obj 订阅的所有消息对应的回调函数
  template <typename HOST_OBJ> void removeCallback(HOST_OBJ *host_obj) {
    for (auto iter = msg_callback_vector_.begin();
         iter != msg_callback_vector_.end();) {
      if (std::get<kHostObj>(*iter) ==
          reinterpret_cast<std::int64_t>(host_obj)) {
        iter = msg_callback_vector_.erase(iter);
      } else {
        ++iter;
      }
    }
  }

  template <typename _MSG_ID> void dispatchMsg(typename _MSG_ID::MsgType *msg) {
    for (auto & iter : msg_callback_vector_) {
      std::get<kMsgCallback> (iter)(static_cast<EasyMsg *>(msg));
    }
  }

private:
  std::vector<MsgBindTupe>::iterator find_callback(void *host_obj,
                                                   const std::string &msgid) {
    for (auto iter = msg_callback_vector_.begin();
         iter != msg_callback_vector_.end(); ++iter) {
      if ((std::get<kHostObj>(*iter) ==
           reinterpret_cast<std::int64_t>(host_obj)) &&
          (std::get<kMsgId>(*iter) == msgid)) {
        return iter;
      }
    }
    return std::end(msg_callback_vector_);
  }

private:
  //消息订阅关系
  std::vector<MsgBindTupe> msg_callback_vector_;
};

} // namespace em

#endif // EASYMSGDISPATCHER_H
