#include "hand.h"

namespace sheepshead {
namespace interface {

Hand::Hand(Rules& rules)
{
  m_hand_ptr = std::make_shared<sheepshead::model::Hand> ();
  m_hand_ptr->set_allocated_rule_variation(&rules.m_rule_variation);
}

Hand::Hand(std::istream* input)
{
  m_hand_ptr->ParseFromIstream(input);
}

bool Hand::serialize(std::ostream* output) const
{
  return m_hand_ptr->SerializeToOstream(output);
}

Rules Hand::rules() const
{
  return Rules(m_hand_ptr);
}

Chronicle Hand::chronicle() const
{
  return Chronicle(m_hand_ptr);
}

// TODO: Implement Hand state methods
bool Hand::is_playable() const
{
  return false;
}

bool Hand::is_arbitable() const
{
  return false;
}

bool Hand::is_complete() const
{
  return false;
}

} // namespace interface
} // namespade sheepshead
