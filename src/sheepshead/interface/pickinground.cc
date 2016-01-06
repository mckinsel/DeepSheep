#include "pickinground.h"

#include "sheepshead/interface/rules.h"

#include <algorithm>
#include <sstream>

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
  if(this->is_null()) return PickDecisionItr<Handle_T>();

  auto itr = PickDecisionItr<Handle_T>(m_hand_ptr, 0);
  return itr;
}

template<typename Handle_T>
PickDecisionItr<Handle_T> PickingRound<Handle_T>::pick_decisions_end() const
{
  if(this->is_null()) return PickDecisionItr<Handle_T>();

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
  if(this->is_null()) return PlayerItr();

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
  return LonerDecision::NONE;
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
  if(this->is_null()) return false;

  return m_hand_ptr->picking_round().unknown_decision_made();
}

template<typename Handle_T>
std::vector<Card> PickingRound<Handle_T>::blinds() const
{
  std::vector<Card> output;
  if(this->is_null()) return output;

  for(auto& model_card : m_hand_ptr->picking_round().blinds()) {
    output.push_back(Card(m_hand_ptr, model_card));
  }

  return output;
}

template<typename Handle_T>
std::vector<Card> PickingRound<Handle_T>::discarded_cards() const
{
  std::vector<Card> output;
  if(this->is_null()) return output;

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
  if(this->is_null()) return false;
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
  if(!(this->picker()->is_null()) && // There is a picker
     !(this->loner_decision() == LonerDecision::NONE) && // He's made a loner decision
      (this->unknown_decision_has_been_made()) && // He's made the unknown decision
      (this->discarded_cards().size() ==
          static_cast<size_t>(Rules(m_hand_ptr).number_of_cards_in_blinds())))
                                          // He's discarded
    return true;

  return false;
}

template<typename Handle_T>
PickDecision PickingRound<Handle_T>::pick_decision_by(PlayerId playerid) const
{
  auto player_itr = leader();
  if(player_itr.is_null()) {
    return PickDecision::UNASKED;
  }

  for(auto pick_itr=pick_decisions_begin(); pick_itr!=pick_decisions_end(); pick_itr++) {
    if(*player_itr == playerid) {
      return *pick_itr;
    }
  }

  return PickDecision::UNASKED;
}

template<typename Handle_T>
std::string PickingRound<Handle_T>::debug_string() const
{
  std::stringstream out_str;
  out_str << "Picking round ";
  if(!is_finished()) out_str << "not ";
  out_str << "finished. Player decisions: ";

  for(auto pick_itr=pick_decisions_begin(); pick_itr!=pick_decisions_end(); pick_itr++) {
    if(*pick_itr == PickDecision::PICK) {
      out_str << " PICK ";
    } else if(*pick_itr == PickDecision::PASS) {
      out_str << " PASS ";
    } else if(*pick_itr == PickDecision::UNASKED) {
      out_str << " UNASKED ";
    }
  }
  out_str << std::endl;
  out_str << "Loner decision is ";
  if(loner_decision() == LonerDecision::PARTNER) {
    out_str << "PARTNER";
  } else if(loner_decision() == LonerDecision::LONER) {
    out_str << "LONER";
  } else if(loner_decision() == LonerDecision::NONE) {
    out_str << "NONE";
  }
  
  out_str << std::endl;
  out_str << "Partner card is ";
  out_str << partner_card().debug_string();


  return out_str.str();
}

template class PickingRound<ConstHandHandle>;
template class PickingRound<MutableHandHandle>;


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
  if(m_hand_ptr == nullptr) {
    m_pick_decision = PickDecision::UNASKED;
    return m_pick_decision;
  }
  if(m_decision_number >= m_hand_ptr->picking_round().picking_decisions_size() ||
     m_decision_number < 0) {
      m_pick_decision = PickDecision::UNASKED;
  } else {
   switch(m_hand_ptr->picking_round().picking_decisions(m_decision_number)) {
     case model::PickingRound::PICK :
       m_pick_decision = PickDecision::PICK;
       break;
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
