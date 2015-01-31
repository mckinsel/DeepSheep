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

// TODO: Implement Hand state methods
bool Hand::is_playable() const
{
  return false;
}

bool Hand::is_arbitrable() const
{
  return false;
}

bool Hand::is_complete() const
{
  return false;
}

} // namespace interface
} // namespade sheepshead
