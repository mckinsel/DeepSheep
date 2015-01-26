#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_PICKINGROUND_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_PICKINGROUND_H_

#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/deck.h"
#include "sheepshead/interface/playerid.h"

#include <iterator>

namespace sheepshead {
namespace interface {

class ConstPickDecisionIterator; // defined below

//! The intreface to the picking round of a Hand.

//! The picking round is the round where player either pick or pass. Then,
//! depending on the rules and circumstances, the picking player may choose
//! a partner, designate one of her cards as unknown, and discard cards.
class PickingRound
{
public:
  
  //! An enum for the different picking decisions a player can make.

  //! A player can pick or pass, and it is also possible that someone picked
  //! before he had a chance, in which case, he was unasked.
  enum class PickDecision {PICK, PASS, UNASKED};

  //! An enum for decision about going alone.
  enum class LonerDecision {PARTNER, LONER};

  //! Construct a null PickingRound.
  PickingRound();

  //! Construct an interface to the PickingRound of a Hand.
  PickingRound(const ConstHandHandle& hand_ptr);
  
  //! Return the PlayerId of the player makes the pick/pass decision first.
  PlayerId leader() const;

  //! Return the PlayerId of the player who picked.
  
  //! If no one picked, or no one has picked yet, return the null PlayerId.
  PlayerId picker() const;

  //! Return the card selected by the picker as the partner card.
  Card partner_card() const;
  
  //! Return an iterator to the first pick decision
  ConstPickDecisionIterator pick_decision_cbegin() const;

  //! Return an iterator past the last pick decision
  ConstPickDecisionIterator pick_decision_cend() const;
  
  bool is_null() const;
  bool is_started() const;
  bool is_finished() const;

private:
  const ConstHandHandle m_hand_ptr;

}; // class PickingRound

class ConstPickDecisionIterator
  : public std::iterator<std::input_iterator_tag, PickingRound::PickDecision, int>
{
public:
  bool operator==(const ConstPickDecisionIterator& rhs);
  bool operator!=(const ConstPickDecisionIterator& rhs);

  const PickingRound::PickDecision& operator*();
  ConstPickDecisionIterator& operator++();

private:
  friend class PickingRound;
  ConstPickDecisionIterator(const ConstHandHandle&, const PlayerId&);

  const ConstHandHandle m_hand_ptr;
  PlayerId m_player_id;
  PickingRound::PickDecision m_decision;

}; // class ConstPickDecisionIterator

} // namespace interface
} // namespade sheepshead
#endif
