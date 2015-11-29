#include "playerid.h"

#include "sheepshead/interface/rules.h"

#include <cassert>
#include <sstream>

namespace sheepshead {
namespace interface {

PlayerId::PlayerId() :
  m_hand_ptr(nullptr), m_position(-1)
{}

PlayerId::PlayerId(const ConstHandHandle& hand, int pos) :
  m_hand_ptr(hand), m_position(pos)
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

bool PlayerId::operator<(const PlayerId& other) const
{
  return m_position < other.m_position;
}

std::string PlayerId::debug_string() const
{
  std::ostringstream out_stream;
  out_stream << "Player " <<  m_position;
  return out_stream.str();
}

PlayerItr::PlayerItr()
  : m_hand_ptr(nullptr), m_position(-1)
{}

PlayerItr::PlayerItr(const ConstHandHandle& hand_ptr, int position)
  : m_hand_ptr(hand_ptr), m_position(position)
{
  assert(m_position < Rules(hand_ptr).number_of_players());
  assert(m_position >= 0);
}

PlayerId& PlayerItr::operator*()
{
  if(this->is_null()) {
    m_playerid = PlayerId();
    return m_playerid;
  }

  assert(m_position < Rules(m_hand_ptr).number_of_players());
  assert(m_position >= 0);
  m_playerid = PlayerId(m_hand_ptr, m_position);
  return m_playerid;
}

bool PlayerItr::is_null() const
{
  return m_hand_ptr == nullptr;
}

PlayerId* PlayerItr::operator->()
{
  return &(operator*());
}

PlayerItr& PlayerItr::operator++()
{
  if(this->is_null()) return *this;

  m_position = (m_position + 1) % Rules(m_hand_ptr).number_of_players();
  return *this;
}

PlayerItr PlayerItr::operator++(int)
{
  auto copy(*this);
  ++(*this); 
  return copy;
}

PlayerItr& PlayerItr::operator--()
{
  if(this->is_null()) return *this;

  m_position = (m_position + Rules(m_hand_ptr).number_of_players() - 1)
                  % Rules(m_hand_ptr).number_of_players();
  return *this;
}

PlayerItr PlayerItr::operator--(int)
{
  auto copy(*this);
  --(*this); 
  return copy;
}

bool PlayerItr::operator==(const PlayerItr& rhs) const
{
  return m_hand_ptr == rhs.m_hand_ptr && m_position == rhs.m_position;
}

bool PlayerItr::operator!=(const PlayerItr& rhs) const
{
  return !(*this == rhs);
}

} // namespace interface
} // namespace sheepshead
