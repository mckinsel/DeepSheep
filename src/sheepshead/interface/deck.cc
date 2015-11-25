#include "deck.h"

#include "sheepshead/interface/rules.h"

// Headers for shuffling the deck.
#include <algorithm>
#include <chrono>
#include <random>

#include <iostream>
#include <stdio.h>

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
  std::vector<model::Card> output_cards(m_deck.begin(),
                                        m_deck.begin() + number_of_cards);
  m_deck.erase(m_deck.begin(), m_deck.begin() + number_of_cards);
  return output_cards;
}

} // namespace internal

Card::Card()
  : m_hand_ptr(nullptr)
{
  m_model_card_ptr = decltype(m_model_card_ptr)
                     (new model::Card(model::Card::default_instance()));
}

Card::Card(const ConstHandHandle& hand_ptr,
           const model::Card& card)
  : m_hand_ptr(hand_ptr)
{
  m_model_card_ptr = decltype(m_model_card_ptr)(new model::Card(card));
}

Card::Card(const Card& from)
{
  m_hand_ptr = from.m_hand_ptr;
  m_model_card_ptr = decltype(m_model_card_ptr)
                     (new model::Card(*from.m_model_card_ptr));
}

std::string Card::debug_string() const
{
  if(is_null()) return "None";

  std::string out_string;
  switch (m_model_card_ptr->rank()) {
    case model::SEVEN : out_string += "SEVEN"; break;
    case model::EIGHT : out_string += "EIGHT"; break;
    case model::NINE  : out_string += "NINE"; break;
    case model::TEN   : out_string += "TEN"; break;
    case model::JACK  : out_string += "JACK"; break;
    case model::QUEEN : out_string += "QUEEN"; break;
    case model::KING  : out_string += "KING"; break;
    case model::ACE   : out_string += "ACE"; break;
  }
  out_string += "-";
  switch (m_model_card_ptr->suit()) {
    case model::DIAMONDS : out_string += "DIAMONDS"; break;
    case model::HEARTS   : out_string += "HEARTS"; break;
    case model::SPADES   : out_string += "SPADES"; break;
    case model::CLUBS    : out_string += "CLUBS"; break;
  }
  out_string += "-";
  if(m_model_card_ptr->unknown()) {
    out_string += "true";
  } else {
    out_string += "false";
  }
  return out_string;
}

Card& Card::operator=(const Card& from)
{
  m_hand_ptr = from.m_hand_ptr;
  m_model_card_ptr = decltype(m_model_card_ptr)
                     (new model::Card(*from.m_model_card_ptr));
  return *this;
}

bool Card::operator==(const Card& rhs) const
{
  return m_model_card_ptr->rank() == rhs.m_model_card_ptr->rank() &&
         m_model_card_ptr->suit() == rhs.m_model_card_ptr->suit();
}

bool Card::operator!=(const Card& rhs) const
{
  return !(*this == rhs);
}

bool Card::is_null() const
{
  return m_hand_ptr == nullptr;
}

bool Card::is_unknown() const
{
  return m_model_card_ptr->unknown();
}

Card::Suit Card::suit() const
{
  // If the card is the unknown card, report the partner suit as its suit.
  if(this->is_unknown()) {
    model::Suit partner_suit = m_hand_ptr->picking_round().partner_card().suit();
    switch(partner_suit) {
      case model::DIAMONDS : return Card::Suit::DIAMONDS;
      case model::HEARTS : return Card::Suit::HEARTS;
      case model::CLUBS : return Card::Suit::CLUBS;
      case model::SPADES : return Card::Suit::SPADES;
    }
  }

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
  switch(m_model_card_ptr->suit()) {
    case model::DIAMONDS : return Card::Suit::DIAMONDS;
    case model::HEARTS : return Card::Suit::HEARTS;
    case model::CLUBS : return Card::Suit::CLUBS;
    case model::SPADES : return Card::Suit::SPADES;
  }
  return Card::Suit::UNKNOWN;
}

Card::Rank Card::rank() const
{
  if(this->is_unknown()) return Card::Rank::UNKNOWN;
  return this->true_rank();
}

Card::Rank Card::true_rank() const
{
  switch(m_model_card_ptr->rank()) {
    case model::ACE : return Card::Rank::ACE;
    case model::TEN : return Card::Rank::TEN;
    case model::KING : return Card::Rank::KING;
    case model::QUEEN : return Card::Rank::QUEEN;
    case model::JACK : return Card::Rank::JACK;
    case model::NINE : return Card::Rank::NINE;
    case model::EIGHT : return Card::Rank::EIGHT;
    case model::SEVEN : return Card::Rank::SEVEN;
  }
  return Card::Rank::UNKNOWN;
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

// CardItr

CardItr::CardItr()
  : m_hand_ptr(nullptr)
{}

CardItr::CardItr(const ConstHandHandle& hand_ptr,
                 const ModelCardItr& model_card_itr)
  : m_model_card_itr(model_card_itr), m_hand_ptr(hand_ptr)
{}

bool CardItr::is_null() const
{
  return m_hand_ptr == nullptr;
}

CardItr& CardItr::operator++()
{
  if(this->is_null()) return *this;

  m_model_card_itr++;
  return *this;
}

CardItr CardItr::operator++(int)
{
  auto copy(*this);
  ++(*this);
  return copy;
}

CardItr& CardItr::operator--()
{
  if(this->is_null()) return *this;

  m_model_card_itr--;
  return *this;
}

CardItr CardItr::operator--(int)
{
  auto copy(*this);
  --(*this);
  return copy;
}

Card& CardItr::operator*()
{
  m_card = Card(m_hand_ptr, *m_model_card_itr);
  return m_card;
}

Card* CardItr::operator->()
{
  return &(operator*());
}

bool CardItr::operator==(const CardItr& rhs) const
{
  return m_hand_ptr == rhs.m_hand_ptr &&
         m_model_card_itr == rhs.m_model_card_itr;
}

bool CardItr::operator!=(const CardItr& rhs) const
{
  return !(*this == rhs);
}

} // namespace interface
} // namespace sheepshead
