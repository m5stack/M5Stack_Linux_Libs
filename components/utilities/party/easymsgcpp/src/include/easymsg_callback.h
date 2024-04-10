#ifndef EASYMSGCALBACK_H
#define EASYMSGCALBACK_H
#include "easymsg_export.h"
#include <functional>

namespace em {
class EasyMsg;
class EASYMSG_API EasyMsgCallback {
public:
  EasyMsgCallback(std::function<void(em::EasyMsg *)> func);
  void operator()(em::EasyMsg *easymsg);

private:
  std::function<void(em::EasyMsg *)> easymsg_callback_;
};

} // namespace em

#endif // EASYMSGCALBACK_H
