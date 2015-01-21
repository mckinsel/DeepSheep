#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_TRICK_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_TRICK_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/playerid.h"

#include <memory>

namespace sheepshead {
namespace interface {

class Trick
{
public:
  Trick(const ConstHandHandle& hand_ptr, int trick_number); 
  
  bool is_started() const; 
  bool is_finished() const;

  PlayerId leader() const;
  PlayerId winner() const;

  //Card card_played_nth(int) const;
  //Card card_played_by_player(PlayerId) const;

private:
  const ConstHandHandle m_hand_ptr;
  const int m_trick_number;

}; // class Trick

} // namespace interface
} // namespace sheepshead

#endif
