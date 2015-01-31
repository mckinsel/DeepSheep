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
PickDecisionItr<Handle_T> PickingRound<Handle_T>::pick_decisions_begin() const
{
  auto itr = PickDecisionItr<Handle_T>(m_hand_ptr, 0);
  return itr;
}

template<typename Handle_T>
PickDecisionItr<Handle_T> PickingRound<Handle_T>::pick_decisions_end() const
{
  auto itr = PickDecisionItr<Handle_T>(m_hand_ptr, Rules(m_hand_ptr).number_of_players());
  return itr;
}

template<typename Handle_T>
PlayerItr PickingRound<Handle_T>::leader() const
{
  if(this->is_null()) return PlayerItr();

  return PlayerItr(m_hand_ptr, m_hand_ptr->picking_round().leader_position());
}

template<typename Handle_T>
PlayerItr PickingRound<Handle_T>::picker() const
{
  auto first = this->pick_decisions_begin();
  auto last = this->pick_decisions_end();
  auto player_itr = this->leader();

  while(first != last) {
    if((*first) == PickDecision::PICK) break;
    ++first;
    ++player_itr;
  }

  if(first == last) return PlayerItr();

  return player_itr;
}

template<typename Handle_T>
LonerDecision PickingRound<Handle_T>::loner_decision() const
{
  if(this->is_null()) return LonerDecision::NONE;
  if(!m_hand_ptr->picking_round().has_loner_decision()) return LonerDecision::NONE;

  switch(m_hand_ptr->picking_round().loner_decision()) {
    case model::PickingRound::PARTNER : return LonerDecision::PARTNER;
    case model::PickingRound::LONER : return LonerDecision::LONER;
  }
}

template<typename Handle_T>
Card PickingRound<Handle_T>::partner_card() const
{
  if(this->is_null()) return Card();
  if(!m_hand_ptr->picking_round().has_partner_card()) return Card();

  return Card(m_hand_ptr, m_hand_ptr->picking_round().partner_card());
}

template<typename Handle_T>
bool PickingRound<Handle_T>::unknown_decision_has_been_made() const
{
  return m_hand_ptr->picking_round().has_unknown_decision_made();
}

template<typename Handle_T>
std::vector<Card> PickingRound<Handle_T>::discarded_cards() const
{
  std::vector<Card> output;

  for(auto& model_card : m_hand_ptr->picking_round().discarded_cards()) {
    output.push_back(Card(m_hand_ptr, model_card));
  }

  return output;
}

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

template<typename Handle_T>
bool PickingRound<Handle_T>::is_finished() const
{
  if(!this->is_started()) return false;

  // It could be that everyone passed
  if(m_hand_ptr->picking_round().picking_decisions_size() ==
      Rules(m_hand_ptr).number_of_players()) {
    if(std::all_of(this->pick_decisions_begin(), this->pick_decisions_end(),
                   [](PickDecision p){return p == PickDecision::PASS;}))
      return true;
  }

  // Or someone has picked and made all decisions
  if(!((*this->picker()).is_null()) &&
     !(this->loner_decision() == LonerDecision::NONE) &&
      (this->unknown_decision_has_been_made()) &&
     !(this->partner_card().is_null()) &&
      (this->discarded_cards().size() ==
          static_cast<size_t>(Rules(m_hand_ptr).number_of_cards_in_blinds())))
    return true;

  return false;
}

template class PickingRound<ConstHandHandle>;
template class PickingRound<MutableHandHandle>;


template<typename Handle_T>
PickDecisionItr<Handle_T>::PickDecisionItr()  : m_hand_ptr(nullptr), m_decision_number(-1)
  : m_hand_ptr(nullptr), m_decision_number(-1)
{}

template<typename Handle_T>
PickDecisionItr<Handle_T>::PickDecisionItr(const Handle_T& hand_ptr, int decision_number)
  : m_hand_ptr(hand_ptr), m_decision_number(decision_number)
{}

template<typename Handle_T>
PickDecisionItr<Handle_T>& PickDecisionItr<Handle_T>::operator++()
{
  m_decision_number++;
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
  m_decision_number--;
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
  if(m_decision_number >= m_hand_ptr->picking_round().picking_decisions_size() ||
     m_decision_number < 0) {
      m_pick_decision = PickDecision::UNASKED;
  } else {
   switch(m_hand_ptr->picking_round().picking_decisions(m_decision_number)) {
     case model::PickingRound::PICK :
       m_pick_decision = PickDecision::PICK;
     case model::PickingRound::PASS :
       m_pick_decision = PickDecision::PASS;
    }
  }

  return m_pick_decision;
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
} // namespace sheepshead
