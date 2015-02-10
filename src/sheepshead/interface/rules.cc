#include "rules.h"

namespace sheepshead {
namespace interface {

Rules::Rules(const ConstHandHandle& hand_ptr)
  : m_hand_ptr(hand_ptr)
{}

int Rules::number_of_players() const
{
  return m_hand_ptr->rule_variation().num_players();
}

int Rules::number_of_cards_per_player() const
{
  int num_cards = 0;

  switch(m_hand_ptr->rule_variation().num_players()) {
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

  switch(m_hand_ptr->rule_variation().num_players()) {
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

bool Rules::partner_is_allowed() const
{
  return m_hand_ptr->rule_variation().num_players() > 4;
}

bool Rules::partner_by_called_ace() const
{
  return m_hand_ptr->rule_variation().partner_method() == sheepshead::model::CALLED_ACE;
}

bool Rules::partner_by_jack_of_diamonds() const
{
  return m_hand_ptr->rule_variation().partner_method() == sheepshead::model::JACK_OF_DIAMONDS;
}

bool Rules::no_picker_leasters() const
{
  return m_hand_ptr->rule_variation().no_picker_result() == sheepshead::model::LEASTERS;
}

bool Rules::no_picker_doubler() const
{
  return m_hand_ptr->rule_variation().no_picker_result() == sheepshead::model::DOUBLER;
}

bool Rules::no_picker_forced_pick() const
{
  return m_hand_ptr->rule_variation().no_picker_result() == sheepshead::model::FORCED_PICK;
}

bool Rules::trump_is_diamonds() const
{
  return m_hand_ptr->rule_variation().trump_suit() == sheepshead::model::DIAMONDS;
}

bool Rules::trump_is_clubs() const
{
  return m_hand_ptr->rule_variation().trump_suit() == sheepshead::model::CLUBS;
}

bool Rules::order_is_the_spitz() const
{
  return m_hand_ptr->rule_variation().the_spitz();
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

// MutableRules

MutableRules::MutableRules()
{
  m_hand_ptr = std::unique_ptr<model::Hand>(new model::Hand);
}

Rules MutableRules::get_rules() const
{
  auto const_hand_ptr = std::make_shared<const sheepshead::model::Hand>
                                  (*m_hand_ptr);
  return Rules(const_hand_ptr);
}

void MutableRules::set_number_of_players(int number_of_players)
{
  if(3 <= number_of_players && number_of_players <= 5) {
    m_hand_ptr->mutable_rule_variation()->set_num_players(number_of_players);
  }
}

void MutableRules::set_partner_by_called_ace()
{
  m_hand_ptr->mutable_rule_variation()->set_partner_method(model::CALLED_ACE);
}

void MutableRules::set_partner_by_jack_of_diamonds()
{
  m_hand_ptr->mutable_rule_variation()->set_partner_method(model::JACK_OF_DIAMONDS);
}


void MutableRules::set_no_picker_leasters()
{
  m_hand_ptr->mutable_rule_variation()->set_no_picker_result(model::LEASTERS);
}

void MutableRules::set_no_picker_doubler()
{
  m_hand_ptr->mutable_rule_variation()->set_no_picker_result(model::DOUBLER);
}

void MutableRules::set_no_picker_forced_pick()
{
  m_hand_ptr->mutable_rule_variation()->set_no_picker_result(model::FORCED_PICK);
}

void MutableRules::set_trump_is_diamonds()
{
  m_hand_ptr->mutable_rule_variation()->set_trump_suit(model::DIAMONDS);
}

void MutableRules::set_trump_is_clubs()
{
  m_hand_ptr->mutable_rule_variation()->set_trump_suit(model::CLUBS);
}

} // namespace interface
} // namespade sheepshead
