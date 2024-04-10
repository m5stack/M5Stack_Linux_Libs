#ifndef EASYMSG_H
#define EASYMSG_H

#include <functional>
#include <iostream>
#include <string>
#include <utility>

#ifdef ENABLE_BOOST_SERIALIZATION
// add boost serialization
#endif

#include "easymsg_dispatcher.h"
#include "easymsg_export.h"

namespace em {

class EasyMsg;

class EASYMSG_API EasyMsg {
public:
  EasyMsg();
  virtual ~EasyMsg() = default;
  virtual std::string id() const = 0;

  template <typename T> struct is_easymsg {
    template <typename U> static char test(typename U::MsgType *x);
    template <typename U> static long test(U *x);
    static const bool value = sizeof(test<T>(0)) == 1;
  };

  // c++17 support constexpr if
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
  template <typename EASY_MSG_ID> bool match() {
    is_easymsg<EASY_MSG_ID> test_easymsg;
    if constexpr (test_easymsg.value) { // c++17
      return id() == EASY_MSG_ID::value;
    } else {
      std::cerr << "匹配消息ID时发生错误，检查是否使用了未定义的消息？ 检查:"
                << typeid(EASY_MSG_ID).name() << std::endl;
      return false;
    }
  }
#else
  template <class MSGID>
  typename std::enable_if<!is_easymsg<MSGID>::value, bool>::type match() {
    std::cerr
        << "匹配消息ID时发生错误，检查是否使用了未定义的消息？ typeinfo : "
        << typeid(MSGID).name() << std::endl;
    return false;
  }

  template <class MSGID>
  typename std::enable_if<is_easymsg<MSGID>::value, bool>::type match() {
    return id() == MSGID::value;
  }

#endif
};

#define MAKE_MSG_ID_CAT(_MSG_) #_MSG_
#define MSG_ID_STR(_MSG_) MAKE_MSG_ID_CAT(_MSG_)

#define EASY_MSG_DEFINE(_MSG_, _MSG_ID_)                                       \
  struct _MSG_##_MSG : public _MSG_, public em::EasyMsg {                      \
  public:                                                                      \
    _MSG_##_MSG() = default;                                                   \
    _MSG_##_MSG(_MSG_ msg) : _MSG_{std::move(msg)} {}                          \
    std::string id() const { return MSG_ID_STR(_MSG_); }                       \
  };                                                                           \
  static const char *_MSG_ID_##helper{(char *)MSG_ID_STR(_MSG_)};              \
  struct _MSG_ID_ {                                                            \
  public:                                                                      \
    static constexpr const char *const &value{_MSG_ID_##helper};               \
    using MsgType = _MSG_##_MSG;                                               \
  };

/* 上述宏定义过于复杂 下边做一下解释
 * 在最初的设计中 是不存在 _MSG_ID_##_Helper 的，为了完成 _MSG_ID_::value
 * 的定义， 原本的方案是将 value 定义为 constexpr static const char *
 * ，此方案废弃的原因 也非常现实，在c++11中，static constexpr
 * 变量必须在声明时初始化，也必须在类外进行
 * 定义，此处代码时宏定义生成，是无法在cpp中重新进行定义的，但是，也无法在头文件中定义，
 * 否则将会造成重定义错误，如果你使用c++17
 * 那么这将不会造成任何问题，为了兼容，我决定 采用一些取巧的方法来规避这个问题。
 *
 * 首先 _MSG_ID_::value 的设计目的是直接获得一个字符串，但是无需每次都构造
 * _MSG_ID_ 对象
 * 在不违反最初设计的前提下，我将注意力关注在错误的地方————重定义，因此，我将此变量更改
 * 为引用，避免引发重定义问题，引用的目标则是一个文件内的静态变量，由于文件内的静态变量
 * 只对文件内可见，所以不会引发任何问题。
 **/

template <typename EASY_MSG_ID>
auto easymsg_cast(EasyMsg *msg) -> typename EASY_MSG_ID::MsgType * {
  return static_cast<typename EASY_MSG_ID::MsgType *>(msg);
}

extern EASYMSG_API EasyMsgDispatcher ___dispatcher;

template <typename EASY_MSG_ID>
void EASYMSG_API sendMsg(typename EASY_MSG_ID::MsgType *msg) {
  ___dispatcher.dispatchMsg<EASY_MSG_ID>(msg);
}

} // namespace em

#endif // EASYMSG_H
