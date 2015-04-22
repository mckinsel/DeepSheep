#include <gtest/gtest.h>
#include "test_plays.h"
#include "sheepshead/interface/hand.h"

#include <algorithm>
#include <iterator>
#include <utility>

// Test trump always wins
TEST(TestTrickWinner, TestTrumpWins)
{
  auto hand = testplays::TestHand();
  auto leader_itr =
    testplays::advance_default_hand_past_picking_round(&hand, true, 3);
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::TEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::SEVEN), // Winner
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  auto winner = hand.history().tricks_begin()->winner();
  EXPECT_EQ(winner, *std::next(leader_itr, 2));

  mocked_laid_cards = {
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::TEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::JACK), // Winner
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  winner = hand.history().tricks_begin()->winner();
  EXPECT_EQ(winner, *std::next(leader_itr, 3));
}

TEST(TestTrickWinner, TestSuitFollowing)
{
  auto hand = testplays::TestHand();
  auto leader_itr =
    testplays::advance_default_hand_past_picking_round(&hand, true, 0);
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE), // Winner
    std::make_pair(Card::Suit::SPADES, Card::Rank::TEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::KING),
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  auto winner = hand.history().tricks_begin()->winner();
  EXPECT_EQ(winner, *std::next(leader_itr, 0));

  mocked_laid_cards = {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::TEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::KING), // Winner
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  winner = hand.history().tricks_begin()->winner();
  EXPECT_EQ(winner, *std::next(leader_itr, 3));
}

TEST(TestTrickWinner, TestTrumpOrder)
{
  auto hand = testplays::TestHand();
  auto leader_itr =
    testplays::advance_default_hand_past_picking_round(&hand, true, 0);
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN), // Winner
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::JACK)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  auto winner = hand.history().tricks_begin()->winner();
  EXPECT_EQ(winner, *std::next(leader_itr, 2));

  mocked_laid_cards = {
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN), // Winner
    std::make_pair(Card::Suit::CLUBS, Card::Rank::JACK),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::JACK)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  winner = hand.history().tricks_begin()->winner();
  EXPECT_EQ(winner, *std::next(leader_itr, 1));

  mocked_laid_cards = {
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::JACK), // Winner
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::ACE),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::KING),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::TEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::NINE)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  winner = hand.history().tricks_begin()->winner();
  EXPECT_EQ(winner, *std::next(leader_itr, 0));
}

TEST(TestTrickWinner, TestFailOrder)
{
  auto hand = testplays::TestHand();
  auto leader_itr =
    testplays::advance_default_hand_past_picking_round(&hand, true, 0);
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::KING),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN), // Winner
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  auto winner = hand.history().tricks_begin()->winner();
  EXPECT_EQ(winner, *std::next(leader_itr, 2));

  mocked_laid_cards = {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE), // Winner
    std::make_pair(Card::Suit::CLUBS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  winner = hand.history().tricks_begin()->winner();
  EXPECT_EQ(winner, *std::next(leader_itr, 0));
}

TEST(TestTrickWinner, TestAssignNextLeader)
{
  auto hand = testplays::TestHand();
  auto leader_itr =
    testplays::advance_default_hand_past_picking_round(&hand, true, 0);
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::KING),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN), // Winner
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  auto winner = hand.history().tricks_begin()->winner();

  hand.arbiter().arbitrate();
  leader_itr = hand.history().latest_trick().leader();
  EXPECT_EQ(*leader_itr, winner);

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
