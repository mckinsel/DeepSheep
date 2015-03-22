#include <gtest/gtest.h>
#include "test_plays.h"
#include "sheepshead/interface/hand.h"

#include <algorithm>
#include <utility>

TEST(TestDiscards, TestDiscardCountDefaultRulesLoner)
{
  for(int i=0; i<10; ++i) {
    auto hand = sheepshead::interface::Hand();
    hand.arbiter().arbitrate();
    auto player_itr = hand.history().picking_round().leader();

    EXPECT_TRUE(hand.playmaker(*player_itr)
                .make_play(testplays::pick));
    EXPECT_TRUE(hand.playmaker(*player_itr)
                .make_play(testplays::go_alone));

    auto available_plays = hand.playmaker(*player_itr)
                          .available_plays(); 
    
    EXPECT_GT(available_plays.size(), 0);
    EXPECT_EQ(available_plays[0].play_type(),
              sheepshead::interface::Play::PlayType::DISCARD);

    EXPECT_TRUE(hand.playmaker(*player_itr)
                .make_play(available_plays[0]));

    EXPECT_EQ(hand.seat(*player_itr).number_of_held_cards(), 6);
    EXPECT_EQ(hand.history().picking_round().discarded_cards().size(), 2);
    
    EXPECT_TRUE(hand.history().picking_round().is_finished());
  }
}

TEST(TestDiscards, TestDiscardCountDefaultRulesPartner)
{
  for(int i=0; i<50; ++i) {
    auto hand = sheepshead::interface::Hand();
    hand.arbiter().arbitrate();
    auto player_itr = hand.history().picking_round().leader();

    EXPECT_TRUE(hand.playmaker(*player_itr)
                .make_play(testplays::pick));
    EXPECT_TRUE(hand.playmaker(*player_itr)
                .make_play(testplays::get_partner));

    
    auto available_plays = hand.playmaker(*player_itr)
                          .available_plays(); 
    EXPECT_GT(available_plays.size(), 0);
    EXPECT_EQ(available_plays[0].play_type(),
              sheepshead::interface::Play::PlayType::PARTNER);

    EXPECT_TRUE(hand.playmaker(*player_itr)
                .make_play(available_plays[0]));

    available_plays = hand.playmaker(*player_itr)
                      .available_plays(); 

    if(available_plays[0].play_type() ==
        sheepshead::interface::Play::PlayType::UNKNOWN)  {
      EXPECT_TRUE(hand.playmaker(*player_itr)
                  .make_play(available_plays[0]));
      available_plays = hand.playmaker(*player_itr)
                        .available_plays(); 
    }

    EXPECT_GT(available_plays.size(), 0);
    EXPECT_EQ(available_plays[0].play_type(),
              sheepshead::interface::Play::PlayType::DISCARD);

    EXPECT_TRUE(hand.playmaker(*player_itr)
                .make_play(available_plays[0]));

    EXPECT_EQ(hand.seat(*player_itr).number_of_held_cards(), 6);
    EXPECT_EQ(hand.history().picking_round().discarded_cards().size(), 2);
  }
}

TEST(TestDiscards, TestDiscardCountThreePlayer)
{
  auto mutable_rules = sheepshead::interface::MutableRules();
  mutable_rules.set_number_of_players(3);
  auto hand = sheepshead::interface::Hand(mutable_rules.get_rules());
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();
  
  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::pick));

  // Recall the three player games permit no partner
  
  auto available_plays = hand.playmaker(*player_itr)
                        .available_plays(); 
  EXPECT_GT(available_plays.size(), 0);
  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::DISCARD);

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(available_plays[0]));

  EXPECT_EQ(hand.seat(*player_itr).number_of_held_cards(), 10);
  EXPECT_EQ(hand.history().picking_round().discarded_cards().size(), 2);
}

TEST(TestDiscards, TestDiscardCountFourPlayer)
{
  auto mutable_rules = sheepshead::interface::MutableRules();
  mutable_rules.set_number_of_players(4);
  auto hand = sheepshead::interface::Hand(mutable_rules.get_rules());
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();
  
  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::pick));

  // Recall the four player games permit no partner
  
  auto available_plays = hand.playmaker(*player_itr)
                        .available_plays(); 
  EXPECT_GT(available_plays.size(), 0);
  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::DISCARD);

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(available_plays[0]));

  EXPECT_EQ(hand.seat(*player_itr).number_of_held_cards(), 7);
  EXPECT_EQ(hand.history().picking_round().discarded_cards().size(), 4);
}

// Don't discard the only fail card you have
TEST(TestDiscards, TestDoNotDiscardOnlyFail)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();
  
  // Only possible partner call is ace of spades, and you only have one of that
  // suit 
  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds  {
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::ACE),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::NINE)};
  hand.mock_blinds(mocked_blinds);
  
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::JACK),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::JACK),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT)};
  hand.mock_held_cards(*player_itr, mocked_held_cards);

  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  auto available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::PARTNER);
  EXPECT_EQ(available_plays[0].partner_decision()->suit(),
            sheepshead::interface::Card::Suit::SPADES);
  EXPECT_EQ(available_plays[0].partner_decision()->rank(),
            sheepshead::interface::Card::Rank::ACE);


  hand.playmaker(*player_itr).make_play(available_plays[0]);
  available_plays = hand.playmaker(*player_itr).available_plays();

  // Every pair of cards that doesn't have the eight of spaces is allowed, so
  // we want 7 choose 2 = 21 options
  EXPECT_EQ(available_plays.size(), 21);
  for(auto play : available_plays) {
    EXPECT_TRUE(std::none_of(play.discard_decision()->begin(),
                             play.discard_decision()->end(),
            [](Card c){return c.suit() == Card::Suit::SPADES &&
                              c.rank() == Card::Rank::EIGHT;}));
  }

}

// Don't discard the only two fail cards you have
TEST(TestDiscards, TestDoNotDiscardBothFail)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();
  
  // Only possible partner call is ace of spades, and you only have one of that
  // suit 
  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds  {
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::ACE),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::NINE)};
  hand.mock_blinds(mocked_blinds);
  
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::JACK),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT)};
  hand.mock_held_cards(*player_itr, mocked_held_cards);

  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  auto available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::PARTNER);
  EXPECT_EQ(available_plays[0].partner_decision()->suit(),
            sheepshead::interface::Card::Suit::SPADES);
  EXPECT_EQ(available_plays[0].partner_decision()->rank(),
            sheepshead::interface::Card::Rank::ACE);


  hand.playmaker(*player_itr).make_play(available_plays[0]);
  available_plays = hand.playmaker(*player_itr).available_plays();

  // The only forbidden pair is both spade fails, so we want C(8,2) - 1 = 27
  EXPECT_EQ(available_plays.size(), 27);
  for(auto play : available_plays) {
    EXPECT_FALSE(std::all_of(play.discard_decision()->begin(),
                             play.discard_decision()->end(),
            [](Card c){return c.suit() == Card::Suit::SPADES;}));
  }
}

// Loners aren't constrained in what they discard
TEST(TestDiscards, TestLonerAnythingGoes)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();
  
  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds  {
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::ACE),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::NINE)};
  hand.mock_blinds(mocked_blinds);
  
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::JACK),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::JACK),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT)};
  hand.mock_held_cards(*player_itr, mocked_held_cards);

  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::go_alone);

  auto available_plays = hand.playmaker(*player_itr).available_plays();

  // The only forbidden pair is both spade fails, so we want C(8,2) - 1 = 27
  EXPECT_EQ(available_plays.size(), 28);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
