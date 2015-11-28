#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_ARBITER_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_ARBITER_H_

#include "sheepshead/interface/handle_types.h"

namespace sheepshead {
namespace interface {

class Arbiter
{
public:

  void arbitrate();

  bool is_playable() const;
  bool is_arbitrable() const;
  bool is_finished() const;

private:
  friend class Hand;
  Arbiter(const MutableHandHandle& hand_ptr, unsigned long random_seed);
  MutableHandHandle m_hand_ptr;
  unsigned long m_random_seed;

}; // class Arbiter

} // namespace interface
} // namespace sheepshead

#endif
