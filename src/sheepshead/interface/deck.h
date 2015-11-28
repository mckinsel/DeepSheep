#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_DECK_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_DECK_H_

#include "sheepshead/proto/deck.pb.h"
#include "sheepshead/interface/handle_types.h"

#include <functional>

namespace sheepshead {
namespace interface {
namespace internal {

/// A deck of cards.

/** Only used internally during hand initialization. */
class Deck
{
public:
  /// Set the Deck to have all 32 sheepshead cards.
  void initialize_full_deck();

  /// Put the Deck in a random order.
  void shuffle_deck(unsigned p_seed = 0);

  /// Empty the Deck.
  void clear();
  
  /// Return number_of_cards cards and remove them from the Deck.
  std::vector<model::Card> deal(int number_of_cards);

private:
  std::vector<model::Card> m_deck;

}; // class Deck

} // namespace internal


/// Represents a single card in the context of the rules of sheepshead.
class Card
{
public:
  Card();
  Card(const ConstHandHandle& hand_ptr,
       const model::Card& card);
  Card(const Card& from);
  Card& operator=(const Card& from);
  Card(Card&& from) = default;
  Card& operator=(Card&& from) = default;

  bool operator==(const Card& rhs) const;
  bool operator!=(const Card& rhs) const;

  enum class Suit {DIAMONDS, HEARTS, CLUBS,
                   SPADES, TRUMP, UNKNOWN};
  enum class Rank {ACE, TEN, KING, QUEEN, JACK, NINE,
                   EIGHT, SEVEN, UNKNOWN};
  
  std::string debug_string() const;

  bool is_null() const;

  /// The sheepshead suit, handling unknown and trump
  Suit suit() const;
  /// The sheepshead rank, handling unknown
  Rank rank() const;

  /// The suit printed on the card, no sheepshead variations
  Suit true_suit() const;
  /// The rank printed on the card, no sheepshead variations
  Rank true_rank() const;

  bool is_trump() const;
  bool is_unknown() const;

  int point_value() const;

private:
  ConstHandHandle m_hand_ptr;
  std::unique_ptr<const model::Card> m_model_card_ptr;

}; // class Card


/// An iterator over Cards.
class CardItr
  : public std::iterator<std::bidirectional_iterator_tag,
                         Card, int>
{
  using ModelCardItr =
    ::google::protobuf::RepeatedPtrField<model::Card>
    ::const_iterator;

public:
  CardItr();
  CardItr(const ConstHandHandle& hand_ptr,
          const ModelCardItr& model_card_itr);

  CardItr(const CardItr& from) = default;
  CardItr& operator=(const CardItr& from) = default;
  CardItr(CardItr&& from) = default;
  CardItr& operator=(CardItr&& from) = default;


  bool is_null() const;

  CardItr& operator++();
  CardItr operator++(int);
  CardItr& operator--();
  CardItr operator--(int);

  Card& operator*();
  Card* operator->();

  bool operator==(const CardItr& rhs) const;
  bool operator!=(const CardItr& rhs) const;

private:
  // The iterator we're wrapping
  ModelCardItr m_model_card_itr;

  // Required for the construction of a Card
  ConstHandHandle m_hand_ptr;

  // Holds the dereference value.
  Card m_card;

}; // class CardItr

} // namespace interface
} // namespace sheepshead

// Make Cards hashable. This helps a lot with testing.
namespace std {

template <>
struct hash<::sheepshead::interface::Card>
{
  size_t operator()(const ::sheepshead::interface::Card& card) const
  {
    return std::hash<int>()(static_cast<int>(card.true_suit())) ^
           std::hash<int>()(static_cast<int>(card.true_rank()));
  }
};

} //namespace std
#endif
