#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_HANDSTATE_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_HANDSTATE_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/history.h"
#include "sheepshead/interface/rules.h"

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

bool is_finished(const ConstHandHandle& hand_ptr);
bool is_arbitrable(const ConstHandHandle& hand_ptr);
bool is_playable(const ConstHandHandle& hand_ptr);

// Arbitrable states
bool is_uninitialized(const ConstHandHandle& hand_ptr);
bool ready_for_next_trick(const ConstHandHandle& hand_ptr);

// Playable states
PlayerId ready_for_pick_play(const ConstHandHandle& hand_ptr);
PlayerId ready_for_loner_play(const ConstHandHandle& hand_ptr);
PlayerId ready_for_partner_play(const ConstHandHandle& hand_ptr);
PlayerId ready_for_unknown_play(const ConstHandHandle& hand_ptr);
PlayerId ready_for_discard_play(const ConstHandHandle& hand_ptr);
PlayerId ready_for_trick_play(const ConstHandHandle& hand_ptr);

} // namespace internal
} // namespace interface
} // namespace sheepshead
#endif
