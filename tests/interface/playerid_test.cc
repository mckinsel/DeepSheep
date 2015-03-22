#include <gtest/gtest.h>
#include "sheepshead/interface/hand.h"
#include "sheepshead/interface/playerid.h"

#include <iterator>

// Test that the default constructor creates a null PlayerId
TEST(TestPlayerId, TestDefaultConstructor)
{
  auto playerid = sheepshead::interface::PlayerId();
  EXPECT_TRUE(playerid.is_null());
}

// Test the the PlayerItr behaves like a circular iterator: advancing by the
// number of players in the game gets you to the same player.
TEST(TestPlayerId, TestCircular)
{
  auto hand = sheepshead::interface::Hand();
  auto itr = hand.dealer();

  auto dealerid = *itr;
  EXPECT_FALSE((dealerid).is_null());
  
  auto num_players = hand.rules().number_of_players();

  for(int i = 0; i < num_players; i++) {
    EXPECT_EQ(itr, std::next(itr, num_players));
    EXPECT_EQ(*itr, *(std::next(itr, num_players)));

    for(int j = 1; j < num_players; j++) {
      EXPECT_NE(itr, std::next(itr, j));
      EXPECT_NE(*itr, *(std::next(itr, j)));
    }

    ++itr;
    EXPECT_FALSE((*itr).is_null());
  }
}

TEST(TestPlayerId, TestCopy)
{
  auto hand = sheepshead::interface::Hand();
  auto itr = hand.dealer();
  auto itr_copy = itr;

  auto dealerid = *itr;
  auto dealerid_copy = *itr_copy;

  EXPECT_FALSE((dealerid).is_null());
  EXPECT_FALSE((dealerid_copy).is_null());

  EXPECT_TRUE(dealerid == dealerid_copy);
  EXPECT_FALSE(dealerid != dealerid_copy);

  itr_copy++;
  EXPECT_FALSE(itr == itr_copy);
}

TEST(TestPlayerId, TestBidirectional)
{
  auto hand = sheepshead::interface::Hand();
  auto itr = hand.dealer();
  auto num_players = hand.rules().number_of_players();

  EXPECT_TRUE(itr == std::prev(itr, num_players));
  EXPECT_TRUE(std::next(itr, 1) == std::prev(itr, num_players - 1));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
