#include "trick.h"

#include "sheepshead/interface/rules.h"

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace sheepshead {
namespace interface {

// Trick

template<typename Handle_T>
Trick<Handle_T>::Trick()
  : m_hand_ptr(nullptr), m_trick_number(-1)
{}

template<typename Handle_T>
bool Trick<Handle_T>::is_null() const
{
  return m_hand_ptr == nullptr || m_trick_number < 0;
}

template<typename Handle_T>
bool Trick<Handle_T>::is_started() const
{
  if(this->is_null()) return false;

  if(m_hand_ptr->tricks_size() <= m_trick_number)
    return false;
  
  return m_hand_ptr->tricks_size() > m_trick_number;
}


template<typename Handle_T>
bool Trick<Handle_T>::is_finished() const
{
  if(!this->is_started()) // Checks is_null
    return false;

  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  auto rules = Rules(m_hand_ptr);

  return model_trick.laid_cards_size() == rules.number_of_players();
}

template<typename Handle_T>
PlayerItr Trick<Handle_T>::leader() const
{
  if(!this->is_started()) return PlayerItr(); // Checks is_null

  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  return PlayerItr(m_hand_ptr, model_trick.leader_position());
}

template<typename Handle_T>
CardItr Trick<Handle_T>::laid_cards_begin() const
{
  if(!this->is_started()) return CardItr();

  auto&& laid_cards = m_hand_ptr->tricks(m_trick_number)
                      .laid_cards();
  return CardItr(m_hand_ptr, laid_cards.begin());
}

template<typename Handle_T>
CardItr Trick<Handle_T>::laid_cards_end() const
{
  if(!this->is_started()) return CardItr();

  auto&& laid_cards = m_hand_ptr->tricks(m_trick_number)
                      .laid_cards();
  return CardItr(m_hand_ptr, laid_cards.end());
}

template<typename Handle_T>
int Trick<Handle_T>::number_of_laid_cards() const
{
  if(!this->is_started()) return 0;

  return m_hand_ptr->tricks(m_trick_number).laid_cards_size();
}

template<typename Handle_T>
std::string Trick<Handle_T>::debug_string() const
{
  std::stringstream out_str;
  out_str << "Trick number ";
  out_str << m_trick_number << "; ";
  out_str << "Leader is " << leader()->debug_string() << std::endl;

  for(auto card=laid_cards_begin(); card!=laid_cards_end(); card++) {
    out_str << card->debug_string() << "; ";
  }

  return out_str.str();
}

const std::unordered_map<int, int> TRUMP_RANK_ORDER = {
  std::make_pair((int)Card::Rank::QUEEN, 8),
  std::make_pair((int)Card::Rank::JACK, 7),
  std::make_pair((int)Card::Rank::ACE, 6),
  std::make_pair((int)Card::Rank::TEN, 5),
  std::make_pair((int)Card::Rank::KING, 4),
  std::make_pair((int)Card::Rank::NINE, 3),
  std::make_pair((int)Card::Rank::EIGHT, 2),
  std::make_pair((int)Card::Rank::SEVEN, 1)
};

const std::unordered_map<int, int> TRUMP_QUEEN_JACK_ORDER = {
  std::make_pair((int)Card::Suit::CLUBS, 4),
  std::make_pair((int)Card::Suit::SPADES, 3),
  std::make_pair((int)Card::Suit::HEARTS, 2),
  std::make_pair((int)Card::Suit::DIAMONDS, 1)
};

const std::unordered_map<int, int> FAIL_RANK_ORDER = {
  std::make_pair((int)Card::Rank::ACE, 8),
  std::make_pair((int)Card::Rank::TEN, 7),
  std::make_pair((int)Card::Rank::KING, 6),
  std::make_pair((int)Card::Rank::NINE, 5),
  std::make_pair((int)Card::Rank::EIGHT, 2),
  std::make_pair((int)Card::Rank::SEVEN, 1),
  std::make_pair((int)Card::Rank::UNKNOWN, 0)
};

// Returns true if card_1 beats card_2
bool card_beats(const Card& card_1, const Card& card_2,
                 const Card::Suit& led_suit)
{
  if(card_1.is_null()) return false;
  if(card_2.is_null()) return true;

  // Trump always beats fail
  if(card_1.is_trump() && !card_2.is_trump()) return true;
  if(card_2.is_trump() && !card_1.is_trump()) return false;

  // If both are trump, use the trump orders to determine who wins
  if(card_1.is_trump() && card_2.is_trump()) {
    if(TRUMP_RANK_ORDER.at((int)card_1.rank()) > TRUMP_RANK_ORDER.at((int)card_2.rank()))
      return true;
    if(TRUMP_RANK_ORDER.at((int)card_1.rank()) < TRUMP_RANK_ORDER.at((int)card_2.rank()))
      return false;

    // The trump rank orders are equal, so it must be a queen or a jack
    if(TRUMP_QUEEN_JACK_ORDER.at((int)card_1.true_suit()) >
        TRUMP_QUEEN_JACK_ORDER.at((int)card_2.true_suit()))
      return true;
    if(TRUMP_QUEEN_JACK_ORDER.at((int)card_1.true_suit()) <
        TRUMP_QUEEN_JACK_ORDER.at((int)card_2.true_suit()))
      return false;
  }

  // So neither card is trump, so now see if one followed suit while the other
  // did not
  if(card_1.suit() == led_suit && card_2.suit() != led_suit)
    return true;
  if(card_2.suit() == led_suit && card_1.suit() != led_suit)
    return false;

  // If both followed suit, then use fail rank order
  if(card_1.suit() == led_suit && card_2.suit() == led_suit) {
    if(FAIL_RANK_ORDER.at((int)card_1.rank()) > FAIL_RANK_ORDER.at((int)card_2.rank()))
      return true;
    if(FAIL_RANK_ORDER.at((int)card_1.rank()) < FAIL_RANK_ORDER.at((int)card_2.rank()))
      return false;
  }

  return false;
}

template<typename Handle_T>
PlayerId Trick<Handle_T>::winner() const
{
  if(!(this->is_finished())) return PlayerId();

  auto led_suit = laid_cards_begin()->suit();
  auto winning_card = Card();
  auto winning_player = PlayerId();
  auto player_itr = leader();

  for(auto laid_card_itr=laid_cards_begin(); laid_card_itr!=laid_cards_end(); ++laid_card_itr) {
    if(card_beats(*laid_card_itr, winning_card, led_suit)) {
      winning_player = *player_itr;
      winning_card = *laid_card_itr;
    }
    ++player_itr;
  }
  
  return winning_player;
}


template class Trick<ConstHandHandle>;
template class Trick<MutableHandHandle>;

//TrickItr
template<typename Handle_T>
TrickItr<Handle_T>::TrickItr()
  : m_hand_ptr(nullptr), m_trick_number(-1)
{}

template<typename Handle_T>
TrickItr<Handle_T>::TrickItr(const Handle_T& hand_ptr, int trick_number)
  : m_hand_ptr(hand_ptr), m_trick_number(trick_number)
{}

template<typename Handle_T>
TrickItr<Handle_T>& TrickItr<Handle_T>::operator++()
{
  m_trick_number++;
  return *this; 
} 

template<typename Handle_T>
TrickItr<Handle_T> TrickItr<Handle_T>::operator++(int)
{
  auto copy(*this);
  ++(*this); 
  return copy;
} 

template<typename Handle_T>
TrickItr<Handle_T>& TrickItr<Handle_T>::operator--()
{
  m_trick_number--;
  return *this; 
} 

template<typename Handle_T>
TrickItr<Handle_T> TrickItr<Handle_T>::operator--(int)
{
  auto copy(*this);
  --(*this); 
  return copy;
} 

template<typename Handle_T>
bool TrickItr<Handle_T>::is_null() const
{
  return m_hand_ptr == nullptr;
}

template<typename Handle_T>
Trick<Handle_T>& TrickItr<Handle_T>::operator*()
{
  if(this->is_null()    ||
     m_trick_number < 0 ||
     m_trick_number >= m_hand_ptr->tricks_size())
  {
    m_trick = Trick<Handle_T>();
  } else {
    m_trick = Trick<Handle_T>(m_hand_ptr, m_trick_number);
  }

  return m_trick;
}

template<typename Handle_T>
Trick<Handle_T>* TrickItr<Handle_T>::operator->()
{
  return &(operator*());
}

template<typename Handle_T>
bool TrickItr<Handle_T>::operator==(const TrickItr<Handle_T>& rhs) const
{
  return m_hand_ptr == rhs.m_hand_ptr && m_trick_number == rhs.m_trick_number;
}

template<typename Handle_T>
bool TrickItr<Handle_T>::operator!=(const TrickItr<Handle_T>& rhs) const
{
  return !(*this == rhs);
}

template class TrickItr<ConstHandHandle>;
template class TrickItr<MutableHandHandle>;

} // namespace interface
} // namespace sheepshead
