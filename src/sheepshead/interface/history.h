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
private:
  ConstHandHandle m_hand_ptr;

public:
  History(const ConstHandHandle& hand_ptr); 
  
  //! An interface to the Hand's picking round 
  PickingRound<decltype(m_hand_ptr)>
  picking_round() const;
  
  //! An iterator pointing to the first trick of the Hand.
  TrickItr<decltype(m_hand_ptr)>
  tricks_begin() const;

  //! An iterator pointing past the last trick of the Hand.
  TrickItr<decltype(m_hand_ptr)>
  tricks_end() const;
  
  //! The number of started tricks in the Hand
  int number_of_started_tricks() const;
  
  //! The number of started tricks in the Hand
  int number_of_finished_tricks() const;

  //! The latest Trick being played in the Hand.
  Trick<decltype(m_hand_ptr)> latest_trick() const;


}; // class History

} // namespace interface
} // namespace sheepshead

#endif
