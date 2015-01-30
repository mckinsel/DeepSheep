#include <gtest/gtest.h>
#include "sheepshead/interface/hand.h"
#include "sheepshead/interface/playerid.h"


TEST(TestPlayerId, TestDefaultConstructor)
{
  auto playerid = sheepshead::interface::PlayerId();
  EXPECT_TRUE(playerid.is_null());
}

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
