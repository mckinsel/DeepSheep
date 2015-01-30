#include "pickinground.h"

#include "sheepshead/interface/rules.h"

namespace sheepshead {
namespace interface {

template<typename Handle_T>
PickingRound<Handle_T>::PickingRound()
  : m_hand_ptr(nullptr)
{}

template<typename Handle_T>
PickingRound<Handle_T>::PickingRound(const Handle_T& hand_ptr)
  : m_hand_ptr(hand_ptr)
{}

template<typename Handle_T>
bool PickingRound<Handle_T>::is_null() const
{
  return m_hand_ptr == nullptr;
}

template<typename Handle_T>
bool PickingRound<Handle_T>::is_started() const
{
  return m_hand_ptr->has_picking_round();
}

template class PickingRound<ConstHandHandle>;
template class PickingRound<MutableHandHandle>;

template<typename Handle_T>
PickDecisionItr<Handle_T> PickingRound<Handle_T>::pick_decisions_begin() const
{
  auto itr = PickDecisionItr<Handle_T>(m_hand_ptr, 0);
  return itr;
}

/*PlayerId PickingRound::picker() const
{
  auto first = this->pick_decision_cbegin(); 
  auto last = this->pick_decision_cend(); 

  auto picker_it = std::find_if(first, last,
                                [&](decltype(*first) decision)
                                {return decision == PickDecision::PICK;});

  if(picker_it = last) return PlayerId();
  
  return PlayerId(m_hand_ptr, picker_it->m_player_number);
}*/


template<typename Handle_T>
PickDecisionItr<Handle_T>::PickDecisionItr()
  : m_hand_ptr(nullptr), m_decision_number(-1)
{}

template<typename Handle_T>
PickDecisionItr<Handle_T>::PickDecisionItr(const Handle_T& hand_ptr, int decision_number)
  : m_hand_ptr(hand_ptr), m_decision_number(decision_number)
{}

template<typename Handle_T>
PickDecisionItr<Handle_T>& PickDecisionItr<Handle_T>::operator++()
{
  m_decision_number = (m_decision_number + 1) % Rules(m_hand_ptr).number_of_players();
  return *this;
}

template<typename Handle_T>
PickDecisionItr<Handle_T> PickDecisionItr<Handle_T>::operator++(int)
{
  auto copy(*this);
  ++(*this); 
  return copy;
}

template<typename Handle_T>
PickDecisionItr<Handle_T>& PickDecisionItr<Handle_T>::operator--()
{
  m_decision_number = (m_decision_number + Rules(m_hand_ptr).number_of_players())
                        % Rules(m_hand_ptr).number_of_players();
  return *this;
}

template<typename Handle_T>
PickDecisionItr<Handle_T> PickDecisionItr<Handle_T>::operator--(int)
{
  auto copy(*this);
  --(*this); 
  return copy;
}

template<typename Handle_T>
PickDecision& PickDecisionItr<Handle_T>::operator*()
{
  if(m_decision_number >= m_hand_ptr->picking_round().picking_decisions_size()) {
      m_pick_decision = PickDecision::UNASKED;
  } else {
   switch(m_hand_ptr->picking_round().picking_decisions(m_decision_number)) {
     case model::PickingRound::PICK :
       m_pick_decision = PickDecision::PICK;
     case model::PickingRound::PASS :
       m_pick_decision = PickDecision::PASS;
    }
  }

  return  m_pick_decision;
}

template<typename Handle_T>
bool PickDecisionItr<Handle_T>::operator==(const PickDecisionItr& rhs) const
{
  return m_hand_ptr == rhs.m_hand_ptr && m_decision_number == rhs.m_decision_number;
}

template<typename Handle_T>
bool PickDecisionItr<Handle_T>::operator!=(const PickDecisionItr& rhs) const
{
  return !(*this == rhs);
}

template class PickDecisionItr<ConstHandHandle>;
template class PickDecisionItr<MutableHandHandle>;

} // namespace interface
} // namespade sheepshead
