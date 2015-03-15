#include "sheepshead/interface/hand.h"
#include "sheepshead/interface/playmaker_available_plays.h"

#include <stdlib.h>

// Store some plays for testing


namespace testplays {

auto pick = sheepshead::interface::Play(
    sheepshead::interface::Play::PlayType::PICK,
    sheepshead::interface::PickDecision::PICK);

auto pass = sheepshead::interface::Play(
    sheepshead::interface::Play::PlayType::PICK,
    sheepshead::interface::PickDecision::PASS);

auto go_alone = sheepshead::interface::Play(
    sheepshead::interface::Play::PlayType::LONER,
    sheepshead::interface::LonerDecision::LONER);

auto get_partner = sheepshead::interface::Play(
    sheepshead::interface::Play::PlayType::LONER,
    sheepshead::interface::LonerDecision::PARTNER);


sheepshead::interface::PlayerItr
advance_default_hand_past_picking_round(
    sheepshead::interface::Hand* hand,
    bool do_get_partner,
    int picker_position)
{
  hand->arbiter().arbitrate();
  auto player_itr = hand->history().picking_round().leader();

  int counter = 0;
  while(counter < picker_position) {
    hand->playmaker(*player_itr).make_play(pass);
    ++counter;
    ++player_itr;
  }

  hand->playmaker(*player_itr).make_play(pick);

  if(do_get_partner) {
    hand->playmaker(*player_itr).make_play(get_partner);
  } else {
    hand->playmaker(*player_itr).make_play(go_alone);
  }

  std::vector<sheepshead::interface::Play> available_plays;
  do {
    available_plays = hand->playmaker(*player_itr).available_plays();
    hand->playmaker(*player_itr).make_play(available_plays[0]);
  } while(available_plays[0].play_type() !=
          sheepshead::interface::Play::PlayType::DISCARD);

  return hand->history().picking_round().leader();
}

using sheepshead::interface::Card;



class TestHand : public sheepshead::interface::Hand
{
public:
  TestHand() : sheepshead::interface::Hand() {}
  TestHand(const sheepshead::interface::Rules& rules) :
    sheepshead::interface::Hand(rules) {}

  void mock_held_cards(sheepshead::interface::PlayerId player_id,
                       std::vector<std::pair<Card::Suit, Card::Rank>> card_ids)
  {
    // Find the position of the player. A little hacky because this is not part
    // of the interface.
    int position = 0;
    sheepshead::interface::PlayerId matching_player_id(m_hand_ptr, position);
    while(matching_player_id != player_id) {
      matching_player_id = sheepshead::interface::PlayerId(m_hand_ptr, ++position);
    }

    auto seat = m_hand_ptr->mutable_seats(position);
    seat->clear_held_cards();

    for(auto card_id : card_ids) {
      auto new_card = seat->add_held_cards();
      new_card->set_unknown(false);
      sheepshead::interface::internal::assign_model_suit(new_card, card_id.first);
      sheepshead::interface::internal::assign_model_rank(new_card, card_id.second);
    }
  }

  void mock_blinds(std::vector<std::pair<Card::Suit, Card::Rank>> card_ids)
  {

    auto picking_round = m_hand_ptr->mutable_picking_round();
    picking_round->clear_blinds();
    for(auto card_id : card_ids) {
      auto new_card = picking_round->add_blinds();
      new_card->set_unknown(false);
      sheepshead::interface::internal::assign_model_suit(new_card, card_id.first);
      sheepshead::interface::internal::assign_model_rank(new_card, card_id.second);
    }
  }

  void mock_laid_cards(int trick_number,
                       std::vector<std::pair<Card::Suit, Card::Rank>> card_ids)
  {
    auto trick = m_hand_ptr->mutable_tricks(trick_number);
    trick->clear_laid_cards();
    for(auto card_id : card_ids) {
      auto new_card = trick->add_laid_cards();
      new_card->set_unknown(false);
      sheepshead::interface::internal::assign_model_suit(new_card, card_id.first);
      sheepshead::interface::internal::assign_model_rank(new_card, card_id.second);
    }
  }
};

} // namespace testplays
