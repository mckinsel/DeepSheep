#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_CHRONICLE_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_CHRONICLE_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/playerid.h"
#include "sheepshead/interface/trick.h"
#include "sheepshead/interface/pickinground.h"
#include "sheepshead/interface/handle_types.h"


namespace sheepshead {
namespace interface {

/// The interface to the history of a Hand.
class History
{
public:
  History(const ConstHandHandle& hand_ptr); 
  
  PickingRound picking_round() const;
  
  //! A const iterator pointing to the first trick of the Hand.
  ConstTrickIterator tricks_cbegin() const;

  //! A const iterator pointing past the last trick of the Hand.
  ConstTrickIterator tricks_cend() const;
  
  //! The number of started tricks in the Hand
  int number_of_tricks() const;

  //! The latest Trick being played in the Hand.
  Trick latest_trick() const;

private:
  const ConstHandHandle m_hand_ptr;

}; // class History

} // namespace interface
} // namespace sheepshead

#endif
