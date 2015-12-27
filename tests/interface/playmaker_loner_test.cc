#include <gtest/gtest.h>
#include "test_plays.h"
#include "sheepshead/interface/hand.h"

#include <algorithm>

// Test that the loner decision is valid when partner calls are allowed.
void check_proper_loner(const std::vector<sheepshead::interface::Play>& plays)
{
  EXPECT_EQ(plays.size(), 2);
  // Both should be of LONER type
  EXPECT_TRUE(plays[0].play_type() ==
                       sheepshead::interface::Play::PlayType::LONER &&
              plays[1].play_type() ==
                       sheepshead::interface::Play::PlayType::LONER);
  int loner_count = std::count_if(plays.begin(), plays.end(),
      [](sheepshead::interface::Play p){return *(p.loner_decision()) ==
         sheepshead::interface::LonerDecision::LONER;});
  int partner_count = std::count_if(plays.begin(), plays.end(),
      [](sheepshead::interface::Play p){return *(p.loner_decision()) ==
         sheepshead::interface::LonerDecision::PARTNER;});
  EXPECT_EQ(loner_count, 1);
  EXPECT_EQ(partner_count, 1);
}

// Test that picking transitions the hand to requiring a loner decision
TEST(TestPlaymaker, TestPickLeadsToLoner)
{
  auto hand = sheepshead::interface::Hand();
  hand.arbiter().arbitrate();

  auto player_itr = hand.history().picking_round().leader();
  EXPECT_TRUE(hand.playmaker(*player_itr).make_play(testplays::pick));

  EXPECT_EQ(hand.seat(*player_itr).number_of_held_cards(), 8);
  EXPECT_EQ(hand.history().picking_round().blinds().size(), 0);

  EXPECT_TRUE(hand.is_playable());

  auto available_plays = hand.available_plays(*player_itr);
  check_proper_loner(available_plays);

  // Now try again, but not having the leader pick
  auto second_hand = sheepshead::interface::Hand();
  second_hand.arbiter().arbitrate();

  player_itr = second_hand.history().picking_round().leader();
  EXPECT_TRUE(second_hand.playmaker(*player_itr).make_play(testplays::pass));

  ++player_itr;
  EXPECT_TRUE(second_hand.playmaker(*player_itr).make_play(testplays::pick));
  EXPECT_EQ(second_hand.seat(*player_itr).number_of_held_cards(), 8);
  EXPECT_EQ(second_hand.history().picking_round().blinds().size(), 0);
  EXPECT_TRUE(second_hand.is_playable());
  available_plays = second_hand.available_plays(*player_itr);
  available_plays = second_hand.available_plays(*player_itr);
  check_proper_loner(available_plays);
}

// Test that a picker with the jack of diamonds can only go alone when partner
// is by jack of diamonds
TEST(TestPlaymaker, TestPickerWithJDMustGoAlone)
{
  auto mutable_rules = sheepshead::interface::MutableRules();
  mutable_rules.set_partner_by_jack_of_diamonds();
  auto hand = sheepshead::interface::Hand(mutable_rules.get_rules());
  hand.arbiter().arbitrate();

  auto player_itr = hand.history().picking_round().leader();
  
  // Just keep dealing until the leader has the JD
  while(true) {
    player_itr = hand.history().picking_round().leader();
    EXPECT_TRUE(hand.playmaker(*player_itr).make_play(testplays::pick));
    EXPECT_TRUE(hand.is_playable());

    auto available_plays = hand.available_plays(*player_itr);

    bool has_jd = std::any_of(hand.seat(*player_itr).held_cards_begin(),
                              hand.seat(*player_itr).held_cards_end(),
            [](sheepshead::interface::Card c)
              {return c.true_suit() == sheepshead::interface::Card::Suit::DIAMONDS &&
                      c.true_rank() == sheepshead::interface::Card::Rank::JACK;});
    if(has_jd) {
      EXPECT_EQ(available_plays.size(), 1);
      EXPECT_EQ(available_plays[0], testplays::go_alone);
      break;
    } else {
      EXPECT_EQ(available_plays.size(), 2);
      EXPECT_EQ(available_plays[0].play_type(),
                sheepshead::interface::Play::PlayType::LONER);
    }
    hand = sheepshead::interface::Hand(mutable_rules.get_rules());
    hand.arbiter().arbitrate();
  }
}

// Test that choosing to get a partner leads to having to pick a partner card
// when the rules say called ace
TEST(TestPlaymaker, TestPartnerLonerToPartnerCard)
{
  auto hand = sheepshead::interface::Hand();
  hand.arbiter().arbitrate();

  auto player_itr = hand.history().picking_round().leader();
  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  EXPECT_TRUE(hand.is_playable());

  auto available_plays = hand.available_plays(*player_itr);
  EXPECT_GT(available_plays.size(), 0);
  EXPECT_LE(available_plays.size(), 3);

  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::PARTNER);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
