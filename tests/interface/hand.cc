#include <gtest/gtest.h>
#include "sheepshead/interface/hand.h"
#include "sheepshead/interface/trick.h"

// Test the expected state of a hand that's been default constructed.
TEST(TestHandInterface, TestDefaultConstructor)
{
  auto hand = sheepshead::interface::Hand();
  auto&& history = hand.history();

  auto&& picking_round = history.picking_round();

  EXPECT_FALSE(picking_round.is_null());
  EXPECT_FALSE(picking_round.is_started());
  EXPECT_FALSE(picking_round.is_finished());
  
  EXPECT_EQ(history.number_of_started_tricks(), 0);

  auto trick_itr = history.tricks_cbegin();
  auto&& trick = *trick_itr;
  EXPECT_FALSE(trick.is_started());
}
