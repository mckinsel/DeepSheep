#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_TRICK_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_TRICK_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/playerid.h"
#include "sheepshead/interface/deck.h"

#include <iterator>
#include <memory>
#include <type_traits>

namespace sheepshead {
namespace interface {

/// The interface to a single trick.
template<typename Handle_T>
class Trick
{
public:
  //! Construct an interface to a null trick.
  Trick();

  Trick(const Trick& from) = default;
  Trick& operator=(const Trick& from) = default;

  //! Construct an interface to the trick_numberth trick of a hand.
  Trick(const Handle_T& hand_ptr, int trick_number)
    : m_hand_ptr(hand_ptr), m_trick_number(trick_number) {}

  //! Return true if the Trick wraps a null trick, a conceptually non-existent trick.
  bool is_null() const;

  /// Return true if the Trick has started.

  //! Even if no cards for the trick have been played yet, will
  //! return true if the trick has been created in the model.
  bool is_started() const;

  //! Return true if all cards for the Trick have been played.
  bool is_finished() const;

  //! Return an iterator pointing to the player who led.
  PlayerItr leader() const;

  //! Return an iterator pointing to the first card played in the trick.
  CardItr laid_cards_begin() const;

  //! Return an iterator pointing past the end of cards played in the trick.
  CardItr laid_cards_end() const;

  int number_of_laid_cards() const;

  //! Return the PlayerId of the player who won the Trick.

  //! Return a null player if the Trick is not finished.
  PlayerId winner() const;

  //! Return the card played by the specified player
  Card card_played_by(PlayerId playerid) const;

  //! Return the point value of the trick.
  int point_value(bool include_unknown) const;

  //! Return a string useful for debugging.
  std::string debug_string() const;

private:
  Handle_T m_hand_ptr;
  int m_trick_number;

}; // class Trick


/// An iterator over the Tricks of a Hand History.
template<typename Handle_T>
class TrickItr
  : public std::iterator<std::bidirectional_iterator_tag,
                         Trick<Handle_T>, int>
{
public:
  TrickItr();
  TrickItr(const TrickItr& from) = default;
  TrickItr& operator=(const TrickItr& from) = default;

  TrickItr(const Handle_T& hand_ptr, int trick_number);

  bool is_null() const;

  TrickItr& operator++();
  TrickItr operator++(int);
  TrickItr& operator--();
  TrickItr operator--(int);

  Trick<Handle_T>& operator*();
  Trick<Handle_T>* operator->();

  bool operator==(const TrickItr& rhs) const;
  bool operator!=(const TrickItr& rhs) const;

private:
  Handle_T m_hand_ptr;
  int m_trick_number;
  Trick<Handle_T> m_trick;

}; // class TrickItr

// Implementation of TrickItr

} // namespace interface
} // namespace sheepshead

#endif
