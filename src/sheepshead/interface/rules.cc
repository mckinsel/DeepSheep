#include "rules.h"

namespace sheepshead {
namespace interface {

Rules::Rules()
{
  // Just use the defaults defined in the proto file.
}

Rules::Rules(const ConstHandHandle& hand_handle)
{
  m_rule_variation = hand_handle->rule_variation();
}

Rules::Rules(const sheepshead::model::RuleVariation& rule_variation)
{
  m_rule_variation = rule_variation;
}

int Rules::number_of_players() const
{
  return m_rule_variation.num_players();
}

int Rules::number_of_cards_per_trick() const
{
  int num_cards = 0;

  switch(m_rule_variation.num_players()) {
    case 3:
      num_cards = 10;
      break;
    case 4:
      num_cards = 7;
      break;
    case 5:
      num_cards = 6;
      break;
    default:
      num_cards = 0;
  }

  return num_cards;
}
  
int Rules::number_of_cards_in_blinds() const
{
  int num_cards = 0;

  switch(m_rule_variation.num_players()) {
    case 3:
      num_cards = 2;
      break;
    case 4:
      num_cards = 4;
      break;
    case 5:
      num_cards = 2;
      break;
    default:
      num_cards = 0;
  }

  return num_cards;
}

bool Rules::partner_by_called_ace() const
{
  return m_rule_variation.partner_method() == sheepshead::model::CALLED_ACE;
}

bool Rules::partner_by_jack_of_diamonds() const
{
  return m_rule_variation.partner_method() == sheepshead::model::JACK_OF_DIAMONDS;
}

bool Rules::no_picker_leasters() const
{
  return m_rule_variation.no_picker_result() == sheepshead::model::LEASTERS;
}

bool Rules::no_picker_doubler() const
{
  return m_rule_variation.no_picker_result() == sheepshead::model::DOUBLER;
}

bool Rules::no_picker_forced_pick() const
{
  return m_rule_variation.no_picker_result() == sheepshead::model::FORCED_PICK;
}

bool Rules::trump_is_diamonds() const
{
  return m_rule_variation.trump_suit() == sheepshead::model::DIAMONDS;
}

bool Rules::trump_is_clubs() const
{
  return m_rule_variation.trump_suit() == sheepshead::model::CLUBS;
}

bool Rules::order_is_the_spitz() const
{
  return m_rule_variation.the_spitz();
}

// TODO: implement cracking and recracking
bool Rules::crack_allowed() const
{
  return false;
}

bool Rules::recrack_allowed() const
{
  return false;
}

} // namespace interface
} // namespade sheepshead
