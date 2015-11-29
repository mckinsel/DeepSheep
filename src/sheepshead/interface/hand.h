#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_HAND_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_HAND_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/rules.h"
#include "sheepshead/interface/history.h"
#include "sheepshead/interface/playmaker.h"
#include "sheepshead/interface/playerid.h"
#include "sheepshead/interface/arbiter.h"
#include "sheepshead/interface/seat.h"
#include "sheepshead/interface/pickinground.h"

#include <iostream>
#include <memory>
#include <string>

namespace sheepshead {
namespace interface {

/// The interface to a Sheepshead hand, the main Sheepshead interface.

/** Provides information about the basic state of the hand as well as access to
 *  more specialized interfaces for investigating or manipulating the hand.
 */
class Hand
{
public:
  //! Construct a Hand with default rules.
  Hand(unsigned long random_seed = 0);

  //! Construct a Hand with a specified rule variation.
  Hand(const Rules& rules, unsigned long random_seed = 0);

  //! Construct a Hand by reading a previously serialized Hand from an istream.
  Hand(std::istream* input);

  //! Construct a Hand from a Hand serialized to a string.
  Hand(const std::string& input);
  
  //! Serialize the Hand to an ostream.
  bool serialize(std::ostream* output) const;

  //! Serialize the Hand to a string
  bool serialize(std::string* output) const;
  
  //! Return true if the Hand is in the playable state. 
  bool is_playable() const;
  
  //! Return true if the Hand is in the arbitable state. 
  bool is_arbitrable() const;

  //! Return true if the Hand is in the finished state.
  bool is_finished() const;
  
  //! Get an iterator pointing to the player who dealt the Hand.
  PlayerItr dealer() const;

  //! Get a specialized interface to the rule variant used by the Hand.
  Rules rules() const;

  //! Get a specialized interface to the history of play so far in the Hand.
  History history() const;

  //! Get the interface to a seat of the Hand.
  Seat seat(PlayerId playerid) const;

  //! Get a specialized interface for a player to change the state of the Hand.
  Playmaker playmaker(PlayerId);

  //! Get a specialized interface to apply the rules of Sheepshead to the Hand.
  Arbiter arbiter();

  //! Get a string useful for debugging
  std::string debug_string() const;

  //! Get the points awarded to the player at the end of the hand.
  int reward(PlayerId) const;

protected:
  MutableHandHandle m_hand_ptr;

private:
  unsigned long m_random_seed;

}; // class Hand

} // namespace interface
} // namespace sheepshead

#endif
