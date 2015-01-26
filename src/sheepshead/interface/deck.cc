#include "deck.h"

#include "sheepshead/interface/rules.h"

// Headers for shuffling the deck.
#include <algorithm>
#include <chrono>
#include <random>

namespace sheepshead {
namespace interface {
namespace internal {

void Deck::initialize_full_deck()
{
  m_deck.clear();
  for(int suit = model::Suit_MIN; suit <= model::Suit_MAX; suit++) {

    if(!model::Suit_IsValid(suit)) continue;

    for(int rank = model::Rank_MIN; rank <= model::Rank_MAX; rank++) {

      if(!model::Rank_IsValid(rank)) continue;

      auto new_card = model::Card();
      new_card.set_suit(static_cast<model::Suit>(suit));
      new_card.set_rank(static_cast<model::Rank>(rank));
      new_card.set_unknown(false);

      m_deck.push_back(new_card);
    }
  }
}

void Deck::clear()
{
  m_deck.clear();
}

void Deck::shuffle_deck()
{
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(m_deck.begin(), m_deck.end(), std::default_random_engine(seed));
}

std::vector<model::Card> Deck::deal(int number_of_cards)
{
  std::vector<model::Card> output_cards(m_deck.begin(), m_deck.begin() + number_of_cards);
  m_deck.erase(m_deck.begin(), m_deck.begin() + number_of_cards);
  return output_cards;
}

} // namespace internal


Card::Card(const ConstHandHandle& hand_ptr, model::Card card)
  : m_hand_ptr(hand_ptr), m_card_ref(card)
{}

bool Card::is_unknown() const
{
  return m_card_ref.unknown();
}

Card::Suit Card::suit() const
{
  // If the card is the unknown card, just report that as its suit.
  if(this->is_unknown()) return Card::Suit::UNKNOWN;

  // Check the rules to see if the card is trump
  if(this->is_trump()) return Suit::TRUMP;
  
  return this->true_suit();
}

bool Card::is_trump() const
{
  if(this->is_unknown())
    return false;

  if(this->rank() == Card::Rank::QUEEN ||
     this->rank() == Card::Rank::JACK)
  return true;

  auto actual_suit = this->true_suit();
  if(Rules(m_hand_ptr).trump_is_diamonds() && actual_suit == Suit::DIAMONDS)
    return true;

  if(Rules(m_hand_ptr).trump_is_clubs() && actual_suit == Suit::CLUBS)
    return true;

  return false;
}

Card::Suit Card::true_suit() const
{
  switch(m_card_ref.suit()) {
    case model::DIAMONDS : return Card::Suit::DIAMONDS;
    case model::HEARTS : return Card::Suit::HEARTS;
    case model::CLUBS : return Card::Suit::CLUBS;
    case model::SPADES : return Card::Suit::SPADES;
  }
}

Card::Rank Card::rank() const
{
  if(this->is_unknown()) return Card::Rank::UNKNOWN;
  return this->true_rank();
}

Card::Rank Card::true_rank() const
{
  switch(m_card_ref.rank()) {
    case model::ACE : return Card::Rank::ACE;
    case model::TEN : return Card::Rank::TEN;
    case model::KING : return Card::Rank::KING;
    case model::QUEEN : return Card::Rank::QUEEN;
    case model::JACK : return Card::Rank::JACK;
    case model::NINE : return Card::Rank::NINE;
    case model::EIGHT : return Card::Rank::EIGHT;
    case model::SEVEN : return Card::Rank::SEVEN;
  }
}

int Card::point_value() const
{
  switch(this->true_rank()){
    case Rank::ACE : return 11;
    case Rank::TEN : return 10;
    case Rank::KING : return 4;
    case Rank::QUEEN : return 3;
    case Rank::JACK : return 2;
    default: return 0;
  }
}

} // namespace interface
} // namespace sheepshead
