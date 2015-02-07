#include "hand.h"

namespace sheepshead {
namespace interface {

Hand::Hand()
{
  m_hand_ptr = std::make_shared<sheepshead::model::Hand> ();
}

Hand::Hand(const Rules& rules)
{
  m_hand_ptr = std::make_shared<sheepshead::model::Hand> ();

  model::RuleVariation rule_variation(rules.m_hand_ptr->rule_variation());
  m_hand_ptr->set_allocated_rule_variation(&rule_variation);
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
  return Arbiter(m_hand_ptr);
}

bool Hand::is_playable() const
{
  return Arbiter(m_hand_ptr).is_playable();
}

bool Hand::is_arbitrable() const
{
  return Arbiter(m_hand_ptr).is_arbitrable();
}

bool Hand::is_finished() const
{
  return Arbiter(m_hand_ptr).is_finished();
}

} // namespace interface
} // namespade sheepshead
