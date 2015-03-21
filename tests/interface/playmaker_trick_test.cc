#include <gtest/gtest.h>
#include "test_plays.h"
#include "sheepshead/interface/hand.h"

// Test that the leader from the picking round is supposed to make a trick play
// when the picking round is over.
TEST(TestTricks, TestTrickAfterPickingRound)
{
  auto hand = sheepshead::interface::Hand();
  auto leader_id =
    *testplays::advance_default_hand_past_picking_round(&hand, true, 0);
  
  EXPECT_TRUE(hand.is_arbitrable());
  EXPECT_FALSE(hand.is_playable());
  EXPECT_FALSE(hand.is_finished());

  hand.arbiter().arbitrate();

  EXPECT_TRUE(hand.history().picking_round().is_finished()); 
  EXPECT_FALSE(hand.is_arbitrable());
  EXPECT_TRUE(hand.is_playable());
  EXPECT_FALSE(hand.is_finished());

  auto available_plays = hand.playmaker(leader_id).available_plays();
  EXPECT_GT(available_plays.size(), 0);

  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::TRICK_CARD);
}

// Test that the led suit is followed when possible
TEST(TestTricks, TestFollowLedSuit)
{
  auto hand = testplays::TestHand();
  auto leader_itr =
    testplays::advance_default_hand_past_picking_round(&hand, true, 3);
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE)
  };

  hand.mock_laid_cards(0, mocked_laid_cards);
  auto follower_itr = std::next(leader_itr, 1);

  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::JACK),
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE)};
  hand.mock_held_cards(*follower_itr, mocked_held_cards);

  auto available_plays = hand.playmaker(*follower_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 2);

  EXPECT_EQ(1, std::count_if(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.trick_card_decision()->suit() ==
                sheepshead::interface::Card::Suit::SPADES &&
                p.trick_card_decision()->rank() ==
                sheepshead::interface::Card::Rank::NINE;}));
  EXPECT_EQ(1, std::count_if(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.trick_card_decision()->suit() ==
                sheepshead::interface::Card::Suit::SPADES &&
                p.trick_card_decision()->rank() ==
                sheepshead::interface::Card::Rank::SEVEN;}));
}

// Test that all cards are allowed when the led suit cannot be followed
TEST(TestTricks, TestDoNotFollowWhenCannot)
{
  auto hand = testplays::TestHand();
  auto leader_itr =
    testplays::advance_default_hand_past_picking_round(&hand, true, 3);
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE)
  };

  hand.mock_laid_cards(0, mocked_laid_cards);
  auto follower_itr = std::next(leader_itr, 1);

  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::JACK),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::NINE)};
  hand.mock_held_cards(*follower_itr, mocked_held_cards);

  auto available_plays = hand.playmaker(*follower_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 6);
}

// Test that the led suit is followed when it's trump.
TEST(TestTricks, TestFollowTrump)
{
  auto hand = testplays::TestHand();
  auto leader_itr =
    testplays::advance_default_hand_past_picking_round(&hand, true, 3);
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::JACK)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  auto follower_itr = std::next(leader_itr, 1);

  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN), // trump
    std::make_pair(Card::Suit::HEARTS, Card::Rank::JACK), // trump
    std::make_pair(Card::Suit::HEARTS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::NINE)}; //trump
  hand.mock_held_cards(*follower_itr, mocked_held_cards);

  auto available_plays = hand.playmaker(*follower_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 3);

  EXPECT_EQ(1, std::count_if(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.trick_card_decision()->true_suit() ==
                sheepshead::interface::Card::Suit::SPADES &&
                p.trick_card_decision()->suit() ==
                sheepshead::interface::Card::Suit::TRUMP &&
                p.trick_card_decision()->rank() ==
                sheepshead::interface::Card::Rank::QUEEN;}));
  EXPECT_EQ(1, std::count_if(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.trick_card_decision()->true_suit() ==
                sheepshead::interface::Card::Suit::HEARTS &&
                p.trick_card_decision()->suit() ==
                sheepshead::interface::Card::Suit::TRUMP &&
                p.trick_card_decision()->rank() ==
                sheepshead::interface::Card::Rank::JACK;}));
  EXPECT_EQ(1, std::count_if(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.trick_card_decision()->true_suit() ==
                sheepshead::interface::Card::Suit::DIAMONDS &&
                p.trick_card_decision()->suit() ==
                sheepshead::interface::Card::Suit::TRUMP &&
                p.trick_card_decision()->rank() ==
                sheepshead::interface::Card::Rank::NINE;}));
}

// Test that the picker won't fail off the last of the partner suit.
TEST(TestTricks, TestPickerKeepsPartnerFail)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();

  // First player passes
  auto player_itr = hand.history().picking_round().leader();
  hand.playmaker(*player_itr).make_play(testplays::pass);
  player_itr++;

  using sheepshead::interface::Card;
  // Give the second player a hand with one club and lots of fail spades
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::NINE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::QUEEN)};
  hand.mock_held_cards(*player_itr, mocked_held_cards);

  // And some great blinds
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds {
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN)};
  hand.mock_blinds(mocked_blinds);

  // The second player picks and wants a partner
  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  // Then the second player selects ace of clubs as the partner card
  auto available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 2); // spades or clubs
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.partner_decision()->suit() != Card::Suit::CLUBS;}),
      available_plays.end());
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // And discards. He won't discard the nine of clubs because it's his only
  // club
  available_plays = hand.playmaker(*player_itr).available_plays();
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  hand.arbiter().arbitrate();

  // Now, suppose that the nine of hearts is led. The picker doesn't have any
  // hearts, so any card, including the nine of clubs could be played. But the
  // rules should prevent that because it's his only club.
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 5);

  EXPECT_TRUE(std::none_of(available_plays.begin(), available_plays.end(),
      [](const sheepshead::interface::Play& p)
        {return p.trick_card_decision()->suit() == Card::Suit::CLUBS;}));
}

// Test that the parter won't lead partner suit that isn't the partner card.
TEST(TestTricks, TestPartnerDoesNotLeadPartnerSuit)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;

  auto player_itr = hand.history().picking_round().leader();

  // First player is going to be partner because of the ace of clubs
  std::vector<std::pair<Card::Suit, Card::Rank> > p1_mocked_held_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::JACK),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::ACE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE)};
  hand.mock_held_cards(*player_itr, p1_mocked_held_cards);

  // First player passes
  hand.playmaker(*player_itr).make_play(testplays::pass);
  player_itr++;

  // Give the second player a hand with one club and lots of fail spades
  std::vector<std::pair<Card::Suit, Card::Rank> > p2_mocked_held_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::NINE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::QUEEN)};
  hand.mock_held_cards(*player_itr, p2_mocked_held_cards);

  // And some great blinds
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds {
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN)};
  hand.mock_blinds(mocked_blinds);

  // The second player picks and wants a partner
  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  // Then the second player selects ace of clubs as the partner card
  auto available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 2); // spades or clubs
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.partner_decision()->suit() != Card::Suit::CLUBS;}),
      available_plays.end());
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // And discards. He won't discard the nine of clubs because it's his only
  // club
  available_plays = hand.playmaker(*player_itr).available_plays();
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  hand.arbiter().arbitrate();

  // Now, see what the first player can play. This should be anything except a
  // club that isn't the ace of clubs
  player_itr = hand.history().picking_round().leader();
  available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 4);

  EXPECT_TRUE(std::none_of(available_plays.begin(), available_plays.end(),
      [](const sheepshead::interface::Play& p)
          {return p.trick_card_decision()->suit() == Card::Suit::CLUBS &&
                  p.trick_card_decision()->rank() != Card::Rank::ACE;}));
}

// Test that when the partner suit is led, the partner plays the parnter card
TEST(TestTricks, TestPartnerFollowsPartnerSuitWithPartnerCard)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;

  auto player_itr = hand.history().picking_round().leader();

  // First player is going to pick
  std::vector<std::pair<Card::Suit, Card::Rank> > p1_mocked_held_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::NINE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::QUEEN)};
  hand.mock_held_cards(*player_itr, p1_mocked_held_cards);

  ++player_itr;
  // Second player will be partner because of the ace of clubs
  std::vector<std::pair<Card::Suit, Card::Rank> > p2_mocked_held_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::JACK),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::ACE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE)};
  hand.mock_held_cards(*player_itr, p2_mocked_held_cards);
  --player_itr;

  // And some great blinds
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds {
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN)};
  hand.mock_blinds(mocked_blinds);

  // First player picks and wants a partner
  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  // Then the second player selects ace of clubs as the partner card
  auto available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 2); // spades or clubs
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.partner_decision()->suit() != Card::Suit::CLUBS;}),
      available_plays.end());
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // And discards. He won't discard the nine of clubs because it's his only
  // club
  available_plays = hand.playmaker(*player_itr).available_plays();
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  hand.arbiter().arbitrate();

  // Now suppose the first player (the picker) leads a club. The second player
  // (partner) has to play the ace of clubs
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::NINE)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);

  ++player_itr;
  available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(available_plays[0].trick_card_decision()->suit(), Card::Suit::CLUBS);
  EXPECT_EQ(available_plays[0].trick_card_decision()->rank(), Card::Rank::ACE);
}

// TODO: Test that the constraint on the picker is removed when the partner card has
// already been played

// TODO: Test follows suit with unknown correctly

// TODO: Test that picker/partner constraints are not applied when partner is
// jack of diamonds

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
