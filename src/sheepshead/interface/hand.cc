#include "hand.h"

#include <chrono>
#include <sstream>

namespace sheepshead {
namespace interface {

Hand::Hand(unsigned long random_seed)
{
  m_hand_ptr = std::make_shared<sheepshead::model::Hand> ();

  if(random_seed == 0) {
    m_random_seed = std::chrono::system_clock::now().time_since_epoch().count();
  } else {
    m_random_seed = random_seed;
  }
}

Hand::Hand(const Rules& rules, unsigned long random_seed)
{
  m_hand_ptr = std::make_shared<sheepshead::model::Hand> ();

  if(random_seed == 0) {
    m_random_seed = std::chrono::system_clock::now().time_since_epoch().count();
  } else {
    m_random_seed = random_seed;
  }

  auto new_rules = rules.m_hand_ptr->rule_variation();
  auto hand_rules = m_hand_ptr->mutable_rule_variation();
  *hand_rules = new_rules;
}

Hand::Hand(std::istream* input)
{
  m_hand_ptr->ParseFromIstream(input);
}

Hand::Hand(const std::string& input)
{
  m_hand_ptr->ParseFromString(input);
}

bool Hand::serialize(std::ostream* output) const
{
  return m_hand_ptr->SerializeToOstream(output);
}

bool Hand::serialize(std::string* output) const
{
  return m_hand_ptr->SerializeToString(output);
}

PlayerItr Hand::dealer() const
{
  return PlayerItr(m_hand_ptr, 0);
}

Rules Hand::rules() const
{
  return Rules(m_hand_ptr);
}

History Hand::history() const
{
  return History(m_hand_ptr);
}

Seat Hand::seat(PlayerId playerid) const
{
  return Seat(m_hand_ptr, playerid);
}

Playmaker Hand::playmaker(PlayerId playerid)
{
  return Playmaker(m_hand_ptr, playerid);
}

Arbiter Hand::arbiter()
{
  return Arbiter(m_hand_ptr, m_random_seed);
}

bool Hand::is_playable() const
{
  return Arbiter(m_hand_ptr, m_random_seed).is_playable();
}

bool Hand::is_arbitrable() const
{
  return Arbiter(m_hand_ptr, m_random_seed).is_arbitrable();
}

bool Hand::is_finished() const
{
  return Arbiter(m_hand_ptr, m_random_seed).is_finished();
}

std::string Hand::debug_string() const
{
  std::stringstream out_stream;
  out_stream << "Hand dealt by " << dealer()->debug_string() << std::endl;
  if(is_playable()) out_stream << "Hand is playable." << std::endl;
  if(is_arbitrable()) out_stream << "Hand is arbitrable." << std::endl;
  if(is_finished()) out_stream << "Hand is finished." << std::endl;

  out_stream << "Hand history:" << std::endl;
  out_stream << history().debug_string() << std::endl;

  out_stream << "Current seats:" << std::endl;
  auto player_itr = dealer();
  do {
    out_stream << seat(*player_itr).debug_string() << std::endl;
    ++player_itr;
  } while(player_itr != dealer());

  return out_stream.str();
}

} // namespace interface
} // namespade sheepshead
