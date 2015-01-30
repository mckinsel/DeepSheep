#include <gtest/gtest.h>
#include "sheepshead/proto/game.pb.h"

TEST(TestModelHand, TestEmptyOnInit)
{
  auto hand = sheepshead::model::Hand();
  EXPECT_EQ(hand.tricks_size(), 0);

}
