#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYERID_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYERID_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"

#include <iterator>
#include <memory>

namespace sheepshead {
namespace interface {

/// The class representing the way to identify a player of a Hand.
class PlayerId
{
public:
  /// Construct the null player.
  PlayerId();
  PlayerId(const PlayerId& from) = default;
  PlayerId& operator=(const PlayerId& from) = default;

  PlayerId(const ConstHandHandle&, int);

  bool is_null() const;

  bool operator==(const PlayerId& rhs) const;
  bool operator!=(const PlayerId& rhs) const;

  std::string debug_string() const;

private:
  friend class Seat;
  ConstHandHandle m_hand_ptr;
  int m_position;

}; // class PlayerId

/// Iterator over players of a Hand.

/** Note that this iterator is circular; it just wraps around to the first
 *  player when it gets to the end, so there is not past-the-end iterator.
 */
class PlayerItr : 
  public::std::iterator<std::bidirectional_iterator_tag, PlayerId, int>
{
public:
  PlayerItr();
  PlayerItr(const PlayerItr& from) = default;
  PlayerItr& operator=(const PlayerItr& from) = default;

  PlayerItr(const ConstHandHandle& hand_ptr, int position);

  bool is_null() const;

  PlayerId& operator*();
  PlayerId* operator->();

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
