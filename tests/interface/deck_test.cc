#include <gtest/gtest.h>
#include "sheepshead/interface/hand.h"
#include <iostream>

TEST(TestDeck, TestCard)
{
  auto hand = sheepshead::interface::Hand();
  hand.arbiter().arbitrate();
  auto bgn_itr = hand.seat(*hand.dealer()).held_cards_begin();
  auto end_itr = hand.seat(*hand.dealer()).held_cards_end();
  
  auto card1 = *bgn_itr;
  ++bgn_itr;
  auto card2 = *bgn_itr;
  EXPECT_FALSE(card1 == card2);
  auto card3 = card2;
  card2 = card2;
  EXPECT_TRUE(card2 == card3);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
