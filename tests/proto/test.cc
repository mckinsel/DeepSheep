#include <gtest/gtest.h>
#include "sheepshead/proto/game.pb.h"

TEST(TestModelHand, TestEmptyOnInit)
{
  auto hand = sheepshead::model::Hand();
  EXPECT_EQ(hand.tricks_size(), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
