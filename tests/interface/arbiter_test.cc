#include <gtest/gtest.h>
#include <unordered_set>
#include "sheepshead/interface/hand.h"

#include <iostream>

TEST(TestArbiter, TestDeal)
{
  auto hand = sheepshead::interface::Hand();

  EXPECT_TRUE(hand.is_arbitrable());
  EXPECT_FALSE(hand.is_finished());
  EXPECT_FALSE(hand.is_playable());

  auto arbiter = hand.arbiter();
  arbiter.arbitrate();
   
  EXPECT_FALSE(hand.is_arbitrable());
  EXPECT_FALSE(hand.is_finished());
  EXPECT_TRUE(hand.is_playable());
  
  // So now the hand is initialized, and we're going to check that the deal was
  // correct  
  auto dealer = hand.dealer();
  auto cards_per_player = hand.rules().number_of_cards_per_player();
  auto cards_in_blinds = hand.rules().number_of_cards_in_blinds();

  EXPECT_EQ(cards_per_player, 6); // This is the default okay
  EXPECT_EQ(cards_in_blinds, 2);

  auto player_itr(dealer);
  int total_cards = 0;
  std::unordered_set<sheepshead::interface::Card> observed_cards;
  
  // Check that all cards are unique and there are 32 total
  do {
    auto seat = hand.seat(*player_itr);
    EXPECT_EQ(seat.number_of_held_cards(), cards_per_player);

    total_cards += seat.number_of_held_cards();
    auto start_itr = seat.held_cards_begin();
    auto end_itr = seat.held_cards_end();

    for(auto card_itr = start_itr;
             card_itr != end_itr;
             ++card_itr)
    {
      EXPECT_EQ(observed_cards.find(*card_itr), observed_cards.end());    
      EXPECT_FALSE(card_itr->is_unknown());
      observed_cards.emplace(*card_itr);
    }
    ++player_itr;
  } while(player_itr != dealer);
    
  auto blinds = hand.history().picking_round().blinds();
  for(auto& card : blinds) {
    EXPECT_EQ(observed_cards.find(card), observed_cards.end());    
    EXPECT_FALSE(card.is_unknown());
    observed_cards.emplace(card);
  }

  EXPECT_EQ(observed_cards.size(), 32);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
