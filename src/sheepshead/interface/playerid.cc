#include "playerid.h"

#include "sheepshead/interface/rules.h"

namespace sheepshead {
namespace interface {

PlayerId::PlayerId(const ConstHandHandle& hand, int pos) :
  m_hand_ptr(hand), m_position(pos)
{}

PlayerId::PlayerId() :
  m_hand_ptr(nullptr), m_position(-1)
{}

bool PlayerId::is_null() const
{
  return m_hand_ptr == nullptr || m_position < 0;
}

bool PlayerId::operator==(const PlayerId& other) const
{
  if(this->is_null()) return other.is_null();

  return m_position == other.m_position && m_hand_ptr == other.m_hand_ptr;
}

bool PlayerId::operator!=(const PlayerId& other) const
{
  return !(*this == other);
}  

PlayerId& PlayerId::operator++()
{
  if(this->is_null()) return *this;

  auto rules = Rules(m_hand_ptr);
  int num_players = rules.number_of_players();
  int next_position = (m_position + 1) % num_players;
  m_position = next_position;
  return *this;
}

PlayerId& PlayerId::operator--()
{
  if(this->is_null()) return *this;

  auto rules = Rules(m_hand_ptr);
  int num_players = rules.number_of_players();
  int next_position = (m_position + num_players + 1) % num_players;
  m_position = next_position;
  return *this;
}

} // namespace interface
} // namespace sheepshead
