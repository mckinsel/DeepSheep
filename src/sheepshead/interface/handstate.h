#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_HANDSTATE_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_HANDSTATE_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/history.h"

//! \file handstate.h
//! \brief Header containing function definitions for determining the state of a Hand.

//! \namespace sheepshead
//! \brief The namespace containing everything specific to the game of Sheepshead.

//! \namespace sheepshead::interface
//! \brief The namespace for the external interface to a hand of Sheepshead.

//! \namespace sheepshead::interface::internal
//! \brief The namespace for functions used internally by the interface to
//!        determine correct behavior.
namespace sheepshead {
namespace interface {
namespace internal {

// Forward declarations of functions defined below

// Arbitrable states
bool is_uninitialized(const ConstHandHandle& hand_ptr);
bool ready_for_next_trick(const ConstHandHandle& hand_ptr);

// Playable states
PlayerId ready_for_pick_play(const ConstHandHandle& hand_ptr);
PlayerId ready_for_loner_play(const ConstHandHandle& hand_ptr);
PlayerId ready_for_unknown_play(const ConstHandHandle& hand_ptr);
PlayerId ready_for_discard_play(const ConstHandHandle& hand_ptr);
PlayerId ready_for_trick_play(const ConstHandHandle& hand_ptr);

/// Return true if the Hand is complete, and no more plays or rule applications are possible.
bool is_finished(const ConstHandHandle& hand_ptr)
{
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


PlayerId ready_for_pick_play(const ConstHandHandle& hand_ptr)
{
  auto history = History(hand_ptr);
  
  if(history.picking_round().is_finished()) return PlayerId();

  return PlayerId();
}

} // namespace internal
} // namespace interface
} // namespade sheepshead
#endif
