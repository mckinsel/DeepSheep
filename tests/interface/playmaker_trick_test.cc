#include <gtest/gtest.h>
#include "test_plays.h"
#include "sheepshead/interface/hand.h"

TEST(TestTricks, TestTrickAfterPickingRound)
{
  auto hand = sheepshead::interface::Hand();
  testplays::advance_default_hand_past_picking_round(&hand, true);
  
  EXPECT_TRUE(hand.is_arbitrable());
  EXPECT_FALSE(hand.is_playable());
  EXPECT_FALSE(hand.is_finished());

  hand.arbiter().arbitrate();

  EXPECT_TRUE(hand.history().picking_round().is_finished()); 
  EXPECT_FALSE(hand.is_arbitrable());
  EXPECT_TRUE(hand.is_playable());
  EXPECT_FALSE(hand.is_finished());
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
