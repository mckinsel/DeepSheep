#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYERID_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYERID_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"

#include <iterator>
#include <memory>

namespace sheepshead {
namespace interface {

class PlayerId
{
public:
  PlayerId();
  PlayerId(const PlayerId& from) = default;
  PlayerId& operator=(const PlayerId& from) = default;

  PlayerId(const ConstHandHandle&, int);

  bool is_null() const;

  bool operator==(const PlayerId& rhs) const;
  bool operator!=(const PlayerId& rhs) const;

private:
  ConstHandHandle m_hand_ptr;
  int m_position;

}; // class PlayerId

class PlayerItr : 
  public::std::iterator<std::forward_iterator_tag, PlayerId, int>
{
public:
  PlayerItr();
  PlayerItr(const PlayerItr& from) = default;
  PlayerItr& operator=(const PlayerItr& from) = default;

  PlayerItr(const ConstHandHandle& hand_ptr, int position);
  
  PlayerId& operator*();

  PlayerItr& operator++();
  PlayerItr operator++(int);
  PlayerItr& operator--();
  PlayerItr operator--(int);

  bool operator==(const PlayerItr& rhs) const;
  bool operator!=(const PlayerItr& rhs) const;

private:
  ConstHandHandle m_hand_ptr;
  int m_position;
  PlayerId m_playerid;

};


} // namespace interface
} // namespade sheepshead
#endif
