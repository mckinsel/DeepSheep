#include "arbiter.h"

#include "sheepshead/interface/deck.h"
#include "sheepshead/interface/rules.h"
#include "sheepshead/interface/handstate.h"

#include <iostream>

namespace sheepshead {
namespace interface {

namespace internal {
void initialize_hand(const MutableHandHandle& hand_ptr); // defined below
} // namespace internal

Arbiter::Arbiter(const MutableHandHandle& hand_ptr)
  : m_hand_ptr(hand_ptr)
{}

void Arbiter::arbitrate()
{
  // The first aribtrable state the Hand can be in is the unitialized state.
  // In this state, nothing's happened, and nobody has any cards yet.
  if(internal::is_uninitialized(m_hand_ptr)) {
    internal::initialize_hand(m_hand_ptr);
    return;
  }

}

bool Arbiter::is_playable() const
{
  return internal::is_playable(m_hand_ptr);
}

bool Arbiter::is_finished() const
{
  return internal::is_finished(m_hand_ptr);
}

bool Arbiter::is_arbitrable() const
{
  return  internal::is_arbitrable(m_hand_ptr);
}

namespace internal {

void initialize_hand(const MutableHandHandle& hand_ptr)
{
  // First get the rules so we know how to initialize.
  auto rules = Rules(hand_ptr);
  
  // Initialize the deck.
  auto deck = internal::Deck();
  deck.initialize_full_deck();
  deck.shuffle_deck();
  
  // Construct the seats and the held cards for each seat 
  for(int player = 0; player < rules.number_of_players(); player++) {
    auto new_seat = hand_ptr->add_seats();

    auto dealt_cards = deck.deal(rules.number_of_cards_per_player());

    for(auto& dealt_card : dealt_cards) {
      auto new_card = new_seat->add_held_cards();
      *new_card = dealt_card;
    }
  }
  
  // Create the picking round, and we're playable
  auto picking_round = hand_ptr->mutable_picking_round();
  picking_round->set_leader_position(0); // Pretty much arbitrary

  // Deal cards into the blinds
  auto blind_cards = deck.deal(rules.number_of_cards_in_blinds());
  for(auto& card : blind_cards) {
    auto new_card = hand_ptr->mutable_picking_round()->add_blinds();
    *new_card = card;
  }
}

} // namespace internal    
} // namespace interface
} // namespace sheesphead
    

