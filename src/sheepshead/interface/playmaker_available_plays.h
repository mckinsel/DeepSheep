#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYMAKER_AVAILABLE_PLAYS_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYMAKER_AVAILABLE_PLAYS_H_

#include "sheepshead/proto/game.pb.h"
#include "deck.h"
#include "seat.h"

namespace sheepshead {
namespace interface {
namespace internal {

/// Return the Seat for the player who picked.
Seat get_picker_seat(ConstHandHandle hand_ptr);
 
/// Assign an interface suit to a model card.
void assign_model_suit(model::Card* model_card, Card::Suit suit);
/// Assign an interface rank to a model card.
void assign_model_rank(model::Card* model_card, Card::Rank rank);

/// Return a vector of Cards that the picker could call as partner cards.
std::vector<Card> get_permitted_partner_cards(ConstHandHandle hand_ptr);

/// Return a vector of vectors of permitted discards
std::vector<std::vector<Card>> get_permitted_discards(ConstHandHandle hand_ptr);

} // namespace internal
} // namespace interface
} // namespace sheepshead

#endif
