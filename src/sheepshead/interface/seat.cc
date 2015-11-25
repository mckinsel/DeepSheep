#include "seat.h"

#include <iostream>

namespace sheepshead {
namespace interface {

Seat::Seat()
  : m_hand_ptr(nullptr)
{}

Seat::Seat(const ConstHandHandle& hand_ptr, const PlayerId& playerid)
  : m_hand_ptr(hand_ptr), m_position(playerid.m_position)
{}

bool Seat::is_null() const
{
  return m_hand_ptr == nullptr;
}

std::string Seat::debug_string() const
{
  std::string debug_str = "Seat at position ";
  debug_str += std::to_string(m_position);
  debug_str += " has hand: ";
  for(auto card=held_cards_begin(); card!=held_cards_end(); card++) {
    debug_str += card->debug_string();
    debug_str += ",";
  }
  return debug_str;
}

CardItr Seat::held_cards_begin() const
{
  if(this->is_null()) return CardItr();
  
  auto&& held_cards = m_hand_ptr->seats(m_position).held_cards();
  return CardItr(m_hand_ptr, held_cards.begin());
}

CardItr Seat::held_cards_end() const
{
  if(this->is_null()) return CardItr();
  auto&& held_cards = m_hand_ptr->seats(m_position).held_cards();
  return CardItr(m_hand_ptr, held_cards.end());
}

int Seat::number_of_held_cards() const
{
  if(this->is_null()) return 0;

  return m_hand_ptr->seats(m_position).held_cards_size();
}

} // namespace interface
} // namespace sheepshead
