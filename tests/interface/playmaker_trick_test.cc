#include <gtest/gtest.h>
#include "test_plays.h"
#include "sheepshead/interface/hand.h"

#include <algorithm>
#include <iterator>
#include <utility>

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

  auto available_plays = hand.available_plays(leader_id);
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

  auto available_plays = hand.available_plays(*follower_itr);
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

  // Play a card and verify that the card shows up
  hand.playmaker(*follower_itr).make_play(available_plays[0]);
  EXPECT_EQ(hand.history().latest_trick().number_of_laid_cards(), 2);
  EXPECT_EQ(hand.seat(*follower_itr).number_of_held_cards(), 5);

  // The follower no longer can play
  EXPECT_TRUE(hand.is_playable());
  EXPECT_FALSE(hand.is_arbitrable());
  available_plays = hand.available_plays(*follower_itr);
  EXPECT_EQ(available_plays.size(), 0);
}

// Test that a trick progresses as expected as cards are played
TEST(TestTricks, TestTrickProgession)
{
  auto hand = testplays::TestHand();
  auto leader_itr =
    testplays::advance_default_hand_past_picking_round(&hand, true, 1);

  EXPECT_TRUE(hand.is_arbitrable());
  EXPECT_FALSE(hand.is_playable());

  hand.arbiter().arbitrate();

  auto player_itr = leader_itr;
  std::vector<sheepshead::interface::Play> available_plays;

  do {
    EXPECT_FALSE(hand.is_arbitrable());
    EXPECT_TRUE(hand.is_playable());

    available_plays = hand.available_plays(*player_itr);
    EXPECT_GT(available_plays.size(), 0);
    hand.playmaker(*player_itr).make_play(available_plays[0]);
    EXPECT_EQ(hand.seat(*player_itr).number_of_held_cards(), 5);

    ++player_itr;
    available_plays.clear();

  } while(player_itr != leader_itr);

  EXPECT_EQ(hand.history().latest_trick().number_of_laid_cards(), 5);

  EXPECT_TRUE(hand.is_arbitrable());
  EXPECT_FALSE(hand.is_playable());

  hand.arbiter().arbitrate();

  EXPECT_FALSE(hand.is_arbitrable());
  EXPECT_TRUE(hand.is_playable());
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

  auto available_plays = hand.available_plays(*follower_itr);
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

  auto available_plays = hand.available_plays(*follower_itr);
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
  auto available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 2); // spades or clubs
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.partner_decision()->suit() != Card::Suit::CLUBS;}),
      available_plays.end());
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // And discards. He won't discard the nine of clubs because it's his only
  // club
  available_plays = hand.available_plays(*player_itr);
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  hand.arbiter().arbitrate();

  // Now, suppose that the nine of hearts is led. The picker doesn't have any
  // hearts, so any card, including the nine of clubs could be played. But the
  // rules should prevent that because it's his only club.
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);
  available_plays = hand.available_plays(*player_itr);
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
  auto available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 2); // spades or clubs
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.partner_decision()->suit() != Card::Suit::CLUBS;}),
      available_plays.end());
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // And discards. He won't discard the nine of clubs because it's his only
  // club
  available_plays = hand.available_plays(*player_itr);
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  hand.arbiter().arbitrate();

  // Now, see what the first player can play. This should be anything except a
  // club that isn't the ace of clubs
  player_itr = hand.history().picking_round().leader();
  available_plays = hand.available_plays(*player_itr);
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

  // Then the first player selects ace of clubs as the partner card
  auto available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 2); // spades or clubs
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.partner_decision()->suit() != Card::Suit::CLUBS;}),
      available_plays.end());
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // And discards. He won't discard the nine of clubs because it's his only
  // club
  available_plays = hand.available_plays(*player_itr);
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  hand.arbiter().arbitrate();

  // Now suppose the first player (the picker) leads a club. The second player
  // (partner) has to play the ace of clubs
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::NINE)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);

  ++player_itr;
  available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(available_plays[0].trick_card_decision()->suit(), Card::Suit::CLUBS);
  EXPECT_EQ(available_plays[0].trick_card_decision()->rank(), Card::Rank::ACE);
}

// Test that the constraint on the picker is removed when the partner card has
// already been played
TEST(TestTricks, TestPickerFreedAfterPartnerCardIsPlayed)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;

  auto player_itr = hand.history().picking_round().leader();

  // First player is going to pick
  std::vector<std::pair<Card::Suit, Card::Rank> > p1_mocked_held_cards {
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::NINE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::NINE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::QUEEN)};
  hand.mock_held_cards(*player_itr, p1_mocked_held_cards);

  ++player_itr;
  // Second player will be partner because of the ace of clubs
  std::vector<std::pair<Card::Suit, Card::Rank> > p2_mocked_held_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::SPADES, Card::Rank::JACK),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::TEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::ACE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE)};
  hand.mock_held_cards(*player_itr, p2_mocked_held_cards);
  --player_itr;

  // And some great blinds
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds {
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN)};
  hand.mock_blinds(mocked_blinds);

  // First player picks and wants a partner and select the ace of clubs as the
  // partner card
  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);
  auto available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 1); // clubs is the only available fail
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // Discard two trump
  auto available_discards = hand.available_plays(*player_itr);
  available_discards.erase(std::remove_if(available_discards.begin(), available_discards.end(),
        [](const sheepshead::interface::Play& p)
            {return std::count_if(p.discard_decision()->begin(), p.discard_decision()->end(),
                                  [](const sheepshead::interface::Card& c)
                                      {return c.is_trump();}) != 2;}),
      available_discards.end());
  hand.playmaker(*player_itr).make_play(available_discards[0]);

  hand.arbiter().arbitrate();

  // Now player 1 has two clubs, the seven and eight. Lead the seven to
  // get player 2 to play the ace.
  auto available_trick_cards = hand.available_plays(*player_itr);
  available_trick_cards.erase(std::remove_if(available_trick_cards.begin(), available_trick_cards.end(),
        [](const sheepshead::interface::Play& p)
            {return p.trick_card_decision()->suit() != Card::Suit::CLUBS || p.trick_card_decision()->rank() != Card::Rank::SEVEN;}),
      available_trick_cards.end());
  EXPECT_EQ(available_trick_cards.size(), 1); // seven
  hand.playmaker(*player_itr).make_play(available_trick_cards[0]);

  ++player_itr;
  available_trick_cards = hand.available_plays(*player_itr);
  EXPECT_EQ(available_trick_cards.size(), 1); // just the ace of clubs

  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::KING),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE),
  };
  hand.mock_laid_cards(0, mocked_laid_cards);

  hand.arbiter().arbitrate();
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_second_laid_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::TEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::ACE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::KING),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE),
  };
  hand.mock_laid_cards(1, mocked_second_laid_cards);
  --player_itr;
  available_trick_cards = hand.available_plays(*player_itr);

  // This is the whole point of all this. The picker has one club and no
  // hearts, but he's free to fail off the club now because the ace has already
  // been played.
  EXPECT_EQ(available_trick_cards.size(), 5);

}

// Test that the picker follows suit with the called suit when he has to.
TEST(TestTricks, TestPickerFollowsWithOnlyPartnerSuit)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();

  using sheepshead::interface::Card;

  auto player_itr = hand.history().picking_round().leader();

  // First player is going to pass
  ++player_itr;

  // Second player is going to pick and has only one club
  std::vector<std::pair<Card::Suit, Card::Rank> > p2_mocked_held_cards {
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::NINE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::TEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::QUEEN)};
  hand.mock_held_cards(*player_itr, p2_mocked_held_cards);
  --player_itr;

  // And some great blinds
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds {
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN)};
  hand.mock_blinds(mocked_blinds);

  hand.playmaker(*player_itr).make_play(testplays::pass);
  ++player_itr;
  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  // Call clubs as partner
  auto available_plays = hand.available_plays(*player_itr);
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.partner_decision()->suit() != Card::Suit::CLUBS;}),
      available_plays.end());
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // And discard
  available_plays =  hand.available_plays(*player_itr);
  hand.playmaker(*player_itr).make_play(available_plays[0]);
  hand.arbiter().arbitrate();

  // Now have the first player lead a club
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN)
  };
  hand.mock_laid_cards(0, mocked_laid_cards);

  // The second player should only be able to play his only club, the seven
  available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 1);
}

// Test that the picker follows with the unknown card
TEST(TestTricks, TestPickerFollowsWithUnknown)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();
  using sheepshead::interface::Card;
  auto player_itr = hand.history().picking_round().leader();

  // In this test, player two will pick, call a partner, and have to declare a
  // card unknown

  // Player one's job here is just to pass and then lead a spade
  std::vector<std::pair<Card::Suit, Card::Rank> > p1_mocked_held_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::TEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::KING),
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE)};
  hand.mock_held_cards(*player_itr, p1_mocked_held_cards);

  // Only the ace of spades can be called as the partner card because player
  // two has the other aces. And, player two has no spades, so he will call a
  // spades unknown.
  std::vector<std::pair<Card::Suit, Card::Rank> > p2_mocked_held_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::ACE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::SEVEN)};
  ++player_itr;
  hand.mock_held_cards(*player_itr, p2_mocked_held_cards);
  --player_itr;

  // The blinds also have only hearts and clubs
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::TEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN)};
  hand.mock_blinds(mocked_blinds);

  hand.playmaker(*player_itr).make_play(testplays::pass);
  player_itr++;
  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  auto available_plays = hand.available_plays(*player_itr);
  // Only option is ace of spades
  EXPECT_EQ(available_plays.size(), 1);
  hand.playmaker(*player_itr).make_play(available_plays[0]);
  available_plays = hand.available_plays(*player_itr);

  // Now have to declare a card unknown, the eight of CLUBS
  EXPECT_EQ(available_plays[0].play_type(), sheepshead::interface::Play::PlayType::UNKNOWN);
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.unknown_decision()->first.suit() != Card::Suit::CLUBS ||
                    p.unknown_decision()->first.rank() != Card::Rank::EIGHT;}),
      available_plays.end());
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(available_plays[0].unknown_decision()->first.rank(), Card::Rank::EIGHT);
  EXPECT_EQ(available_plays[0].unknown_decision()->first.suit(), Card::Suit::CLUBS);
  EXPECT_EQ(available_plays[0].unknown_decision()->second, Card::Suit::SPADES);
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // Finally, discard. It doesn't matter what.
  available_plays = hand.available_plays(*player_itr);
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  hand.arbiter().arbitrate();
  --player_itr;

  // Alright, now have player one play a spade. All he has is spades, but he's
  // going to have to play the ace
  available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 1);
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  // Now, it's player 2's turn, and there should be only one option: the eight
  // of clubs that is now a spades unknown
  ++player_itr;
  available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(available_plays[0].trick_card_decision()->true_suit(), Card::Suit::CLUBS);
  EXPECT_EQ(available_plays[0].trick_card_decision()->true_rank(), Card::Rank::EIGHT);
  EXPECT_EQ(available_plays[0].trick_card_decision()->rank(), Card::Rank::UNKNOWN);
  EXPECT_EQ(available_plays[0].trick_card_decision()->suit(), Card::Suit::SPADES);
}

// Test that the unknown card doesn't win a trick
TEST(TestTricks, TestUnknownIsFail)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();
  using sheepshead::interface::Card;
  auto player_itr = hand.history().picking_round().leader();

  // Player one is going to pick, call ace of hearts as partner, and declare
  // the queen of clubs as hearts unknown
  std::vector<std::pair<Card::Suit, Card::Rank> > p1_mocked_held_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::TEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT)};
  hand.mock_held_cards(*player_itr, p1_mocked_held_cards);

  // Player two is going to follow with the ace of hearts
  std::vector<std::pair<Card::Suit, Card::Rank> > p2_mocked_held_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::ACE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::JACK),
    std::make_pair(Card::Suit::SPADES, Card::Rank::JACK)};
  ++player_itr;
  hand.mock_held_cards(*player_itr, p2_mocked_held_cards);
  --player_itr;

  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN)};
  hand.mock_blinds(mocked_blinds);

  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  auto available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(available_plays[0].play_type(), sheepshead::interface::Play::PlayType::PARTNER);
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 8);
  EXPECT_EQ(available_plays[0].play_type(), sheepshead::interface::Play::PlayType::UNKNOWN);
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.unknown_decision()->first.true_suit() != Card::Suit::CLUBS ||
                    p.unknown_decision()->first.rank() != Card::Rank::QUEEN;}),
      available_plays.end());
  EXPECT_EQ(available_plays.size(), 1);

  // Call the unknown and discard
  hand.playmaker(*player_itr).make_play(available_plays[0]);
  available_plays = hand.available_plays(*player_itr);
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  hand.arbiter().arbitrate();

  // Now the first player plays the heart unknown
  available_plays = hand.available_plays(*player_itr);
  available_plays.erase(std::remove_if(available_plays.begin(), available_plays.end(),
        [](const sheepshead::interface::Play& p)
            {return p.trick_card_decision()->suit() != Card::Suit::HEARTS;}),
      available_plays.end());
  EXPECT_EQ(available_plays.size(), 1);
  hand.playmaker(*player_itr).make_play(available_plays[0]);

  ++player_itr;
  available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(available_plays[0].trick_card_decision()->suit(), Card::Suit::HEARTS);
  EXPECT_EQ(available_plays[0].trick_card_decision()->rank(), Card::Rank::ACE);

  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_laid_cards {
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::ACE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::TEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::EIGHT)
  };
  hand.mock_laid_cards(0, mocked_laid_cards, 0);

  hand.arbiter().arbitrate();

  --player_itr; // back to player 0
  available_plays = hand.available_plays(*player_itr);
  EXPECT_EQ(available_plays.size(), 0);
  ++player_itr; // back to player 0
  available_plays = hand.available_plays(*player_itr);
  EXPECT_GT(available_plays.size(), 0);
}

// TODO: Test that picker/partner constraints are not applied when partner is
// jack of diamonds

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
