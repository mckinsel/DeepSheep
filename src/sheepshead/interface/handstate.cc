#include "handstate.h"

namespace sheepshead {
namespace interface {
namespace internal {

/// Return true if the Hand is complete, and no more plays or rule applications are possible.
bool is_finished(const ConstHandHandle& hand_ptr)
{
  // A hand where 1) the picking round is finished 2) nobody picked 3) no
  // pickers are doublers is done
  if(Rules(hand_ptr).no_picker_doubler() &&
     History(hand_ptr).picking_round().is_finished() &&
     History(hand_ptr).picking_round().picker().is_null())
    return true;

  if(Rules(hand_ptr).number_of_cards_per_player() > History(hand_ptr).number_of_finished_tricks())
    return false;
  return History(hand_ptr).latest_trick().is_finished();
}

/// Return true if the Hand needs application of rules by the Arbiter to advance.
bool is_arbitrable(const ConstHandHandle& hand_ptr)
{
  return is_uninitialized(hand_ptr) || ready_for_next_trick(hand_ptr);
}

/// Return true if the Hand needs a decision from a player to advance.
bool is_playable(const ConstHandHandle& hand_ptr)
{
  return ((!ready_for_pick_play(hand_ptr).is_null()) ||
          (!ready_for_loner_play(hand_ptr).is_null()) ||
          (!ready_for_partner_play(hand_ptr).is_null()) ||
          (!ready_for_unknown_play(hand_ptr).is_null()) ||
          (!ready_for_discard_play(hand_ptr).is_null()) ||
          (!ready_for_trick_play(hand_ptr).is_null()));
}

/// Return true if nothing has happened in the game yet: no cards dealt, picked, played, etc.
bool is_uninitialized(const ConstHandHandle& hand_ptr)
{
  return (hand_ptr->seats_size() == 0 &&
          hand_ptr->tricks_size() == 0 &&
          !(hand_ptr->has_picking_round()));
}

/// Return true if the current trick or picking round is finished and a new trick needs to be created.
bool ready_for_next_trick(const ConstHandHandle& hand_ptr)
{
  // The hand could be uninitialized, in which case it's not ready for the next
  // trick.
  if(is_uninitialized(hand_ptr)) return false;

  // The whole hand could be done, in which case it's not ready for the next
  // trick.
  if(is_finished(hand_ptr)) return false;
  
  auto history = History(hand_ptr);

  // If the PickingRound isn't finished, then we're not ready for the next
  // trick. 
  if(!history.picking_round().is_finished()) return false;
  
  // If the PickingRound is finished, and we don't have any tricks yet, then we
  // are ready for the next trick
  if(history.number_of_started_tricks() == 0) return true;
  
  // Finally, if the PickingRound is finished and at least one trick exists,
  // just return if the current trick is finished.
  return history.latest_trick().is_finished();
}

/// Return the PlayerId of the player whose turn it is to pick or pass.

//! Return the null player if no player needs to pick or pass.
PlayerId ready_for_pick_play(const ConstHandHandle& hand_ptr)
{
  if(is_uninitialized(hand_ptr)) return PlayerId();

  auto picking_round = History(hand_ptr).picking_round();

  // Is the whole round over?
  if(picking_round.is_finished()) return PlayerId();

  // No, so everyone could not have passed. Has a player picked yet?
  if(!(*picking_round.picker()).is_null()) return PlayerId();

  // No, so we're waiting for a picker. Which one?
  auto itr = std::next(picking_round.leader(),
                       std::count_if(picking_round.pick_decisions_begin(),
                                     picking_round.pick_decisions_end(),
                                     [](PickDecision p){return p != PickDecision::UNASKED;}));
  return *itr;
}

/// Return the PlayerId of the player who needs to make the loner decision.

//! Return the null player if no player needs decide loner.
PlayerId ready_for_loner_play(const ConstHandHandle& hand_ptr)
{
  if(is_uninitialized(hand_ptr)) return PlayerId();
  auto picking_round = History(hand_ptr).picking_round();
  if(picking_round.is_finished()) return PlayerId();

  // If there's no picker, then we're not ready for a loner play
  if((*picking_round.picker()).is_null()) return PlayerId();

  // There is a picker, so see if he's decided about going alone yet.
  if(picking_round.loner_decision() == LonerDecision::NONE) {
    return  *picking_round.picker();
  } else {
    return PlayerId();
  }
}

/// Return the PlayerId of the player who needs to make the partner card decision.

//! Return the null player if no player needs decide partner.
PlayerId ready_for_partner_play(const ConstHandHandle& hand_ptr)
{
  if(is_uninitialized(hand_ptr)) return PlayerId();
  auto picking_round = History(hand_ptr).picking_round();
  if(picking_round.is_finished()) return PlayerId();

  // If there's no picker, then we're not ready for a partner
  if((*picking_round.picker()).is_null()) return PlayerId();
  // If we haven't made the loner decision yet, we're not ready for a partner
  if(picking_round.loner_decision() == LonerDecision::NONE) return PlayerId();

  // We've made the loner decision, so see if the partner card has been called
  // yet.
  if(picking_round.partner_card().is_null()) {
    return *picking_round.picker();
  } else {
    return PlayerId();
  }
}

PlayerId ready_for_unknown_play(const ConstHandHandle& hand_ptr)
{
  if(is_uninitialized(hand_ptr)) return PlayerId();
  auto picking_round = History(hand_ptr).picking_round();
  if(picking_round.is_finished()) return PlayerId();

  // If there's no picker, then we're not ready for a partner
  if((*picking_round.picker()).is_null()) return PlayerId();
  // If we haven't made the loner decision yet,  we're not ready
  if(picking_round.loner_decision() == LonerDecision::NONE) return PlayerId();
  // If the partner card hasn't been picked yet, we need to do that
  if(picking_round.partner_card().is_null()) return PlayerId();

  if(!picking_round.unknown_decision_has_been_made()) {
    return *picking_round.picker();
  } else {
    return PlayerId();
  }
}

PlayerId ready_for_discard_play(const ConstHandHandle& hand_ptr)
{
  if(is_uninitialized(hand_ptr)) return PlayerId();
  auto picking_round = History(hand_ptr).picking_round();
  if(picking_round.is_finished()) return PlayerId();

  // If there's no picker, then we're not ready for a partner
  if((*picking_round.picker()).is_null()) return PlayerId();
  // If we haven't made the loner decision yet,  we're not ready
  if(picking_round.loner_decision() == LonerDecision::NONE) return PlayerId();
  // If the partner card hasn't been picked yet, we need to do that
  if(picking_round.partner_card().is_null()) return PlayerId();
  // If the unknown decision hasn't been made yet, we can't discard
  if(!picking_round.unknown_decision_has_been_made()) return PlayerId();

  // If the discarded cards are empty, then we do need to discard
  if(picking_round.discarded_cards().size() == 0) {
    return *picking_round.picker();
  } else {
    return PlayerId();
  }
}

PlayerId ready_for_trick_play(const ConstHandHandle& hand_ptr)
{
  if(is_uninitialized(hand_ptr)) return PlayerId();
  if(is_finished(hand_ptr)) return PlayerId();
  
  if(!History(hand_ptr).picking_round().is_finished()) return PlayerId();

  // So the hand isn't over, but the picking round is
  auto latest_trick = History(hand_ptr).latest_trick();
  if(latest_trick.is_null()) return PlayerId();
}

} // namespace internal
} // namespace interface
} // namespace sheepshead
