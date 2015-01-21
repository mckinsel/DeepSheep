#include "playerid.h"

#include "sheepshead/interface/rules.h"

namespace sheepshead {
namespace interface {

PlayerId::PlayerId(const ConstHandHandle& hand, int pos) :
  m_hand_ptr(hand), m_position(pos)
{
}

PlayerId::PlayerId() :
  m_hand_ptr(), m_position(-1)
{
}

bool PlayerId::is_null()
{
  return m_position == -1;
}

bool PlayerId::operator==(const PlayerId& other)
{
  if(this->is_null()) return false;

  return m_position == other.m_position && m_hand_ptr == other.m_hand_ptr;
}

bool PlayerId::operator!=(const PlayerId& other)
{
  return !(*this == other);
}  

PlayerId PlayerId::next()
{
  if(this->is_null()) return PlayerId();

  auto rules = Rules(m_hand_ptr);
  int num_players = rules.number_of_players();
  int next_position = (m_position + 1) % num_players;
  return PlayerId(m_hand_ptr, next_position);
}

PlayerId PlayerId::previous()
{
  if(this->is_null()) return PlayerId();

  auto rules = Rules(m_hand_ptr);
  int num_players = rules.number_of_players();
  int previous_position = (m_position + num_players - 1) % num_players;
  return PlayerId(m_hand_ptr, previous_position);
}

} // namespace interface
} // namespace sheepshead
