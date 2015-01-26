#include "pickinground.h"

namespace sheepshead {
namespace interface {

PickingRound::PickingRound()
  : m_hand_ptr(nullptr)
{}

PickingRound::PickingRound(const ConstHandHandle& hand_ptr)
  : m_hand_ptr(hand_ptr)
{}

PlayerId PickingRound::leader() const
{
  return PlayerId(m_hand_ptr, m_hand_ptr->picking_round.leader());
}

PlayerId PickingRound::picker() const
{
  auto first = this->pick_decision_cbegin(); 
  auto last = this->pick_decision_cend(); 

  auto picker_it = std::find_if(first, last,
                                [&](decltype(*first) decision)
                                {decision == PickingDecision::PICK;});

  if(picker_it = last) return PlayerId();
  
  return PlayerId(m_hand_ptr, picker_it->m_player_number);
}



ConstPickDecisionIterator::ConstPickDecisionIterator(const ConstHandHandle& hand_ptr,
                                                     int decision_number)

} // namespace interface
} // namespade sheepshead
