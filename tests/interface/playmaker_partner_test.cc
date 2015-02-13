#include <gtest/gtest.h>
#include "test_plays.h"
#include "sheepshead/interface/hand.h"

// Test that choosing to go alone leads to a discard decision, not a partner
// decision
TEST(TestPlaymaker, TestGoAloneLeadsToDiscard)
{
  auto hand = sheepshead::interface::Hand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::pick)); 
  EXPECT_TRUE(hand.is_playable());

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::go_alone));
  EXPECT_TRUE(hand.is_playable());

  auto available_plays = hand.playmaker(*player_itr)
                         .available_plays(); 
  EXPECT_GT(available_plays.size(), 0);

  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::DISCARD);
}

// Test that not going alone leads to a partner decision
TEST(TestPlaymaker, TestNoAloneLeadsToPartner)
{
  auto hand = sheepshead::interface::Hand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::pick)); 
  EXPECT_TRUE(hand.is_playable());

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::get_partner));
  EXPECT_TRUE(hand.is_playable());

  auto available_plays = hand.playmaker(*player_itr)
                         .available_plays(); 
  EXPECT_GT(available_plays.size(), 0);

  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::PARTNER);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
