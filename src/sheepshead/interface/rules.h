#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_RULES_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_RULES_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"

namespace sheepshead {
namespace interface {

//class Hand;

class Rules
{
public:
  
  Rules();
  Rules(const ConstHandHandle& hand_handle);
  Rules(const sheepshead::model::RuleVariation& rule_variation);

  int number_of_players() const; 
  int number_of_cards_per_trick() const;
  int number_of_cards_in_blinds() const;

  bool partner_by_called_ace() const;
  bool partner_by_jack_of_diamonds() const;
  
  bool no_picker_leasters() const;
  bool no_picker_doubler() const;
  bool no_picker_forced_pick() const;

  bool trump_is_diamonds() const;
  bool trump_is_clubs() const;

  bool order_is_the_spitz() const;

  bool crack_allowed() const;
  bool recrack_allowed() const;

private:
  friend class Hand;
  sheepshead::model::RuleVariation m_rule_variation;

}; // class Rules

} // namespace interface
} // namespade sheepshead

#endif
