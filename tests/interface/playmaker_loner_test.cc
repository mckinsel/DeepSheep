#include <gtest/gtest.h>
#include "sheepshead/interface/hand.h"

// Test that the loner decision is valid when partner calls are allowed.
void check_proper_loner(const std::vector<sheepshead::interface::Play>& plays)
{
  EXPECT_EQ(plays.size(), 2);
  // Both should be of LONER type
  EXPECT_TRUE(plays[0].play_type() ==
                       sheepshead::interface::Play::PlayType::LONER &&
              plays[1].play_type() ==
                       sheepshead::interface::Play::PlayType::LONER);
  int loner_count = std::count_if(plays.begin(), plays.end(),
      [](sheepshead::interface::Play p){return *(p.loner_decision()) ==
         sheepshead::interface::LonerDecision::LONER;});
  int partner_count = std::count_if(plays.begin(), plays.end(),
      [](sheepshead::interface::Play p){return *(p.loner_decision()) ==
         sheepshead::interface::LonerDecision::PARTNER;});
  EXPECT_EQ(loner_count, 1);
  EXPECT_EQ(partner_count, 1);
}

// Test that picking transitions the hand to requiring a loner decision
TEST(TestPlaymaker, TestPickLeadsToLoner)
{
  auto hand = sheepshead::interface::Hand();
  hand.arbiter().arbitrate();

  auto pick_play = sheepshead::interface::Play(
      sheepshead::interface::Play::PlayType::PICK,
      sheepshead::interface::PickDecision::PICK);
  auto pass_play = sheepshead::interface::Play(
      sheepshead::interface::Play::PlayType::PICK,
      sheepshead::interface::PickDecision::PASS);

  auto player_itr = hand.history().picking_round().leader();
  EXPECT_TRUE(hand.playmaker(*player_itr).make_play(pick_play));
  EXPECT_EQ(hand.seat(*player_itr).number_of_held_cards(), 8);
  EXPECT_EQ(hand.history().picking_round().blinds().size(), 0);
  EXPECT_TRUE(hand.is_playable());
  auto available_plays = hand.playmaker(*player_itr).available_plays();
  check_proper_loner(available_plays);

  // Now try again, but not having the leader pick
  auto second_hand = sheepshead::interface::Hand();
  second_hand.arbiter().arbitrate();
  player_itr = second_hand.history().picking_round().leader();
  EXPECT_TRUE(second_hand.playmaker(*player_itr).make_play(pass_play));
  ++player_itr;
  EXPECT_TRUE(second_hand.playmaker(*player_itr).make_play(pick_play));
  EXPECT_EQ(second_hand.seat(*player_itr).number_of_held_cards(), 8);
  EXPECT_EQ(second_hand.history().picking_round().blinds().size(), 0);
  EXPECT_TRUE(second_hand.is_playable());
  available_plays = second_hand.playmaker(*player_itr).available_plays();
  check_proper_loner(available_plays);
}

// Test that picking when a partner is not allowed does not lead to a loner
// decision, instead straight to discard.
TEST(TestPlaymaker, TestPickNoPartnerLeadsToDiscard)
{
  auto mutable_rules = sheepshead::interface::MutableRules();
  mutable_rules.set_number_of_players(3); // No partner with 3 players
  auto hand = sheepshead::interface::Hand(mutable_rules.get_rules());
  EXPECT_FALSE(hand.rules().partner_is_allowed());
  
  hand.arbiter().arbitrate();
  auto pick_play = sheepshead::interface::Play(
      sheepshead::interface::Play::PlayType::PICK,
      sheepshead::interface::PickDecision::PICK);
  auto player_itr = hand.history().picking_round().leader();
  EXPECT_TRUE(hand.playmaker(*player_itr).make_play(pick_play));
  EXPECT_TRUE(hand.is_playable());

  auto available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays[0].play_type(),
      sheepshead::interface::Play::PlayType::DISCARD);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
