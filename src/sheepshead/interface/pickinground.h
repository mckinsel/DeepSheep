#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_PICKINGROUND_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_PICKINGROUND_H_

#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/deck.h"
#include "sheepshead/interface/playerid.h"

#include <iterator>

namespace sheepshead {
namespace interface {

template<typename Handle_T>
class PickDecisionItr; // defined below
  
//! An enum for the different picking decisions a player can make.

//! A player can pick or pass, and it is also possible that someone picked
//! before he had a chance, in which case, he was unasked.
enum class PickDecision {PICK, PASS, UNASKED};

//! An enum for decision about going alone vs selecting a partner.
enum class LonerDecision {PARTNER, LONER, NONE};

//! The picking round is the round where player either pick or pass. Then,
//! depending on the rules and circumstances, the picking player may choose
//! a partner, designate one of her cards as unknown, and discard cards.
template<typename Handle_T>
class PickingRound
{
public:

  //! Construct a null PickingRound.
  PickingRound();

  //! Construct an interface to the PickingRound of a Hand.
  PickingRound(const Handle_T& hand_ptr);
  
  //! Return an iterator to the first pick decision
  PickDecisionItr<Handle_T> pick_decisions_begin() const;

  //! Return an iterator past the last pick decision
  PickDecisionItr<Handle_T> pick_decisions_end() const;

  //! Return the PlayerId of the player who picked.
  
  //! If no one picked, or no one has picked yet, return the null PlayerId.
  PlayerId picker() const;

  LonerDecision loner_decision() const;
  
  //! Return the card selected by the picker as the partner card.
  Card partner_card() const;

  bool is_null() const;
  bool is_started() const;
  bool is_finished() const;

private:
  Handle_T m_hand_ptr;

}; // class PickingRound

/// An iterator over the picking decisions of a PickingRound
template<typename Handle_T>
class PickDecisionItr
  : public std::iterator<std::forward_iterator_tag, PickDecision, int>
{
public:
  PickDecisionItr();
  PickDecisionItr(const PickDecisionItr& from) = default;
  PickDecisionItr& operator=(const PickDecisionItr& from) = default;
  
  PickDecisionItr(const Handle_T& hand_ptr, int decision_number);
  
  PickDecisionItr& operator++();
  PickDecisionItr operator++(int);
  PickDecisionItr& operator--();
  PickDecisionItr operator--(int);

  PickDecision& operator*();

  bool operator==(const PickDecisionItr& rhs) const;
  bool operator!=(const PickDecisionItr& rhs) const;

private:
  Handle_T m_hand_ptr;
  int m_decision_number;
  PickDecision m_pick_decision;

}; // class PickDecisionItr

} // namespace interface
} // namespade sheepshead
#endif
