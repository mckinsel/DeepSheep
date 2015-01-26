#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_RULES_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_RULES_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"

namespace sheepshead {
namespace interface {

/// The interface to Sheepshead rule variations.
class Rules
{
public:
  
  //! Construct an interface to the default rule variation
  Rules();

  //! Construct an interface to the rules of a hand
  Rules(const ConstHandHandle& hand_handle);
  

  //! Return the number of players in the hand. Default is 5.
  int number_of_players() const; 
  //! Return the number of cards initially dealt to each player. Default is 6.
  int number_of_cards_per_player() const;
  //! Return the number of cards dealt to the blinds. Default is 2
  int number_of_cards_in_blinds() const;

  //! Return whether partner is chosen by called ace. Default is true.
  bool partner_by_called_ace() const;
  //! Return whether parnter is chosen by jack of diamonds. Default is false.
  bool partner_by_jack_of_diamonds() const;
  
  //! Return whether leasters is player no one picks. Default is true.
  bool no_picker_leasters() const;
  //! Return whether a doubler is played after no one picks. Default is false.
  bool no_picker_doubler() const;
  //! Return whether the dealer is forced to pick if no one picks. Default is false.
  bool no_picker_forced_pick() const;
  
  //! Return is the trump suit is diamonds. Default is true.
  bool trump_is_diamonds() const;
  //! Return is the trump suit is clubs. Default is false.
  bool trump_is_clubs() const;

  //! Return if the Spitz card order is used. Default is false.
  bool order_is_the_spitz() const;
  
  //! Return if cracking is allowed after the pick. Default is false.
  bool crack_allowed() const;
  //! Return if recracking is allowed after a crack. Default is false.
  bool recrack_allowed() const;

private:
  friend class Hand;
  friend class RulesBuilder;
  const ConstHandHandle m_hand_ptr;

}; // class Rules

} // namespace interface
} // namespade sheepshead

#endif
