#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYERID_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYERID_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"

#include <iterator>
#include <memory>

namespace sheepshead {
namespace interface {

class PlayerId; //defined below

class PlayerIterator : 
  public::std::iterator<std::forward_iterator_tag, PlayerId, int>
{
public:
  PlayerIterator();
  PlayerIterator(const PlayerIterator& from);
  PlayerIterator& operator=(const PlayerIterator& from);

  bool operator==(const PlayerIterator& rhs) const;
  bool operator!=(const PlayerIterator& rhs) const;
  
  PlayerId& operator*();

  PlayerIterator& operator++();

private:
  PlayerId m_player_id;
}

class PlayerId
{
public:
  PlayerId();
  PlayerId(const ConstHandHandle&, int);

  bool is_null() const;

  bool operator==(const PlayerId& rhs) const;
  bool operator!=(const PlayerId& rhs) const;
  PlayerId& operator++();
  PlayerId& operator--();
  PlayerId&

private:
  const ConstHandHandle m_hand_ptr;
  int m_position;

}; // class PlayerId

} // namespace interface
} // namespade sheepshead
#endif
