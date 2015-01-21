#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_HAND_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_HAND_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/rules.h"
#include "sheepshead/interface/chronicle.h"

#include <iostream>
#include <memory>
#include <string>

namespace sheepshead {
namespace interface {


class Hand
{
public:
  Hand(Rules& rules);
  Hand(std::istream* input);
  
  bool serialize(std::ostream* output) const;
  
  bool is_playable() const;
  bool is_arbitable() const;
  bool is_complete() const;

  Rules rules() const;
  Chronicle chronicle() const;
   
private:
  MutableHandHandle m_hand_ptr;

}; // class Hand

} // namespace interface
} // namespade sheepshead

#endif
