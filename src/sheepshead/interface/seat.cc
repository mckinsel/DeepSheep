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
