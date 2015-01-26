#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_ARBITER_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_ARBITER_H_

#include "sheepshead/interface/handle_types.h"

namespace sheepshead {
namespace interface {

class Arbiter
{
public:

  void arbitrate();

private:
  friend class Hand;
  Arbiter(const MutableHandHandle& hand_ptr);
  const MutableHandHandle m_hand_ptr;

}; // class Arbiter

} // namespace interface
} // namespace sheepshead

#endif
