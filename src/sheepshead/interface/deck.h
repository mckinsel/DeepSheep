#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_DECK_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_DECK_H_

#include "sheepshead/proto/deck.pb.h"
#include "sheepshead/interface/handle_types.h"

namespace sheepshead {
namespace interface {
namespace internal {

class Deck
{
public:
  void initialize_full_deck();
  void shuffle_deck();
  void clear();
  
  std::vector<model::Card> deal(int number_of_cards);

private:
  std::vector<model::Card> m_deck;

}; // class Deck

} // namespace internal

class Card
{
public:
  Card();
  Card(const ConstHandHandle& hand_ptr, model::Card card);

  enum class Suit {DIAMONDS, HEARTS, CLUBS, SPADES, TRUMP, UNKNOWN};
  enum class Rank {ACE, TEN, KING, QUEEN, JACK, NINE, EIGHT, SEVEN, UNKNOWN};
  
  bool is_null() const;

  Suit suit() const;
  Rank rank() const;

  Suit true_suit() const;
  Rank true_rank() const;

  bool is_trump() const;
  bool is_unknown() const;

  int point_value() const;

private:
  const ConstHandHandle m_hand_ptr;
  const model::Card& m_card_ref;

}; // class Card

} // namespace interface
} // namespace sheepshead

#endif
