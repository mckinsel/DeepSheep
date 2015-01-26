#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_TRICK_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_TRICK_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/playerid.h"

#include <iterator>
#include <memory>

namespace sheepshead {
namespace interface {

/// The interface to a single trick.
class Trick
{
public:
  //! Construct an interface to a null trick.
  Trick();

  //! Construct an interface to the trick_numberth trick of a hand.
  Trick(const ConstHandHandle& hand_ptr, int trick_number);
  
  //! Return true  if the Trick wraps a null trick, a conceptually non-existent trick.
  bool is_null();
  
  /// Return true if the Trick has started.

  //! Even if no cards for the trick have been played yet, will return true if
  //! the trick has been created.
  bool is_started() const; 

  //! Return true if all cards for the Trick have been played.
  bool is_finished() const;
  
  //! Return the PlayerId of the player who plays the first card of the Trick.
  PlayerId leader() const;

  //! Return the PlayerId of the player who won the Trick.

  //! Return a null player if the Trick is not finished.
  PlayerId winner() const;

  //Card card_played_by_player(PlayerId) const;

private:
  const ConstHandHandle m_hand_ptr;
  const int m_trick_number;

}; // class Trick


/// An iterator over the Tricks of a hand History.
class ConstTrickIterator
  : public std::iterator<std::input_iterator_tag, Trick, int>
{
public:
  bool operator==(const ConstTrickIterator& rhs) const;
  bool operator!=(const ConstTrickIterator& rhs) const;;
  const Trick& operator*();
  ConstTrickIterator& operator++();

private:
  friend class History;
  ConstTrickIterator(const ConstHandHandle&, int);

  const ConstHandHandle m_hand_ptr;
  int m_trick_number;
  std::unique_ptr<const Trick> m_trick_uptr;

}; // class ConstTrickIterator


} // namespace interface
} // namespace sheepshead

#endif
