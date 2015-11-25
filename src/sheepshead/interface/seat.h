#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_SEAT_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_SEAT_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/deck.h"
#include "sheepshead/interface/playerid.h"

namespace sheepshead {
namespace interface {

class Seat
{
public:
  Seat();
  Seat(const ConstHandHandle& hand_ptr, const PlayerId& playerid);
  Seat(const Seat&) = default;
  Seat& operator=(const Seat&) = default;
  
  bool is_null() const;  

  std::string debug_string() const;

  CardItr held_cards_begin() const;
  CardItr held_cards_end() const;
  int number_of_held_cards() const;

private:
  ConstHandHandle m_hand_ptr;
  int m_position;

}; // class Seat

} // namespace interface
} // namespace sheepshead

#endif
