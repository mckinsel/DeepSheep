#include <gtest/gtest.h>
#include "sheepshead/interface/hand.h"

// Test that upon initialization, only one player has available plays and that
// those plays are pick or pass
TEST(TestPlaymaker, TestPickPassAvailable)
{
  auto hand = sheepshead::interface::Hand();
  hand.arbiter().arbitrate(); // Initialize the hand
  
  using sheepshead::interface::Play;

  auto player_itr(hand.history().picking_round().leader());
  auto leader = *player_itr;

  int counter = 0;
  do {
    auto available_plays = hand.playmaker(*player_itr).available_plays();
    if(*player_itr == leader) {
      EXPECT_EQ(available_plays.size(), 2);
      // Make sure all the plays have the right type
      EXPECT_TRUE(std::all_of(available_plays.begin(), available_plays.end(),
                  [](Play p){return p.play_type() == Play::PlayType::PICK;}));

      // Make sure that there's one play and one pass
      int pass_count = std::count_if(available_plays.begin(), available_plays.end(),
                  [](Play p){return *p.pick_decision() ==
                             sheepshead::interface::PickDecision::PASS;});
      int pick_count = std::count_if(available_plays.begin(), available_plays.end(),
                  [](Play p){return *p.pick_decision() ==
                             sheepshead::interface::PickDecision::PICK;});

      EXPECT_EQ(pick_count, 1);
      EXPECT_EQ(pass_count, 1);

    } else {
      EXPECT_EQ(available_plays.size(), 0);
    }
    ++counter;
    ++player_itr;
  } while(*player_itr != leader);
  EXPECT_EQ(counter, 5);
}

// A helper function that has all players choose to pass
void choose_all_pass(sheepshead::interface::Hand* hand)
{
  hand->arbiter().arbitrate();
  EXPECT_TRUE(hand->is_playable());
  auto player_itr = hand->history().picking_round().leader();
  auto leader = *player_itr;
  do {
    auto available_plays = hand->playmaker(*player_itr).available_plays();
    EXPECT_EQ(available_plays.size(), 2);

    auto pass_play = sheepshead::interface::Play(
                          sheepshead::interface::Play::PlayType::PICK,
                          sheepshead::interface::PickDecision::PASS);
    EXPECT_TRUE(hand->playmaker(*player_itr).make_play(pass_play));
    ++player_itr;
  } while(*player_itr != leader);
}

// Test that the hand becomes arbitrable when everyone passes and the rules say
// leasters
TEST(TestPlaymaker, TestAllPassLeasters)
{
  auto mutable_rules = sheepshead::interface::MutableRules();
  mutable_rules.set_no_picker_leasters();
  auto hand = sheepshead::interface::Hand(mutable_rules.get_rules());

  choose_all_pass(&hand);

  EXPECT_TRUE(hand.history().picking_round().is_finished());
  EXPECT_FALSE(hand.is_playable());
  EXPECT_FALSE(hand.is_finished());
  EXPECT_TRUE(hand.is_arbitrable());
}

// Test that the hand becomes finished when everyone passes and the rules say
// doubler
TEST(TestPlaymaker, TestAllPassDoubler)
{
  auto mutable_rules = sheepshead::interface::MutableRules();
  mutable_rules.set_no_picker_doubler();
  auto hand = sheepshead::interface::Hand(mutable_rules.get_rules());

  choose_all_pass(&hand);

  EXPECT_TRUE(hand.history().picking_round().is_finished());
  EXPECT_FALSE(hand.is_playable());
  EXPECT_TRUE(hand.is_finished());
  EXPECT_FALSE(hand.is_arbitrable());
}

// Test that the last picker is forced to pick when the rules say forced pick
TEST(TestPlaymaker, TestForcedPick)
{
  auto mutable_rules = sheepshead::interface::MutableRules();
  mutable_rules.set_no_picker_forced_pick();
  auto hand = sheepshead::interface::Hand(mutable_rules.get_rules());

  hand.arbiter().arbitrate();
  EXPECT_TRUE(hand.is_playable());
  auto player_itr = hand.history().picking_round().leader();
  auto last_picker = *std::prev(player_itr);
  do {
    auto available_plays = hand.playmaker(*player_itr).available_plays();
    EXPECT_EQ(available_plays.size(), 2);

    auto pass_play = sheepshead::interface::Play(
                          sheepshead::interface::Play::PlayType::PICK,
                          sheepshead::interface::PickDecision::PASS);
    EXPECT_TRUE(hand.playmaker(*player_itr).make_play(pass_play));
    ++player_itr;
  } while(*player_itr != last_picker);

  auto available_plays = hand.playmaker(last_picker).available_plays();
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(*(available_plays[0].pick_decision()),
            sheepshead::interface::PickDecision::PICK);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
