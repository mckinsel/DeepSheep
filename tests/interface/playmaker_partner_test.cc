#include <gtest/gtest.h>
#include "test_plays.h"
#include "sheepshead/interface/hand.h"

#include <algorithm>

// Test that choosing to go alone leads to a discard decision, not a partner
// decision
TEST(TestPlaymaker, TestGoAloneLeadsToDiscard)
{
  auto hand = sheepshead::interface::Hand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::pick)); 
  EXPECT_TRUE(hand.is_playable());

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::go_alone));
  EXPECT_TRUE(hand.is_playable());

  auto available_plays = hand.playmaker(*player_itr)
                         .available_plays(); 
  EXPECT_GT(available_plays.size(), 0);

  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::DISCARD);
}

// Test that not going alone leads to a partner decision
TEST(TestPlaymaker, TestNoAloneLeadsToPartner)
{
  auto hand = sheepshead::interface::Hand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::pick)); 
  EXPECT_TRUE(hand.is_playable());

  EXPECT_TRUE(hand.playmaker(*player_itr)
              .make_play(testplays::get_partner));
  EXPECT_TRUE(hand.is_playable());

  auto available_plays = hand.playmaker(*player_itr)
                         .available_plays(); 
  EXPECT_GT(available_plays.size(), 0);

  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::PARTNER);
}

// Test that choosing to get a partner when the rules say jack of diamonds
// leads to discard
TEST(TestPlaymaker, TestPartnerJDToDiscard)
{
  auto mutable_rules = sheepshead::interface::MutableRules();
  mutable_rules.set_partner_by_jack_of_diamonds();
  auto hand = sheepshead::interface::Hand(mutable_rules.get_rules());
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();

  // Advance until we get to a player without the jack of diamonds
  using sheepshead::interface::Card;
  while(true) {
    auto seat = hand.seat(*player_itr);
    if(std::none_of(seat.held_cards_begin(), seat.held_cards_end(),
          [](Card c){return c.true_suit() == Card::Suit::DIAMONDS &&
                            c.true_rank() == Card::Rank::JACK;})) {
      break;
    }
    hand.playmaker(*player_itr).make_play(testplays::pass);
    ++player_itr;
  }

  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  auto available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_GT(available_plays.size(), 0);
  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::DISCARD);
}

// Verify that the card is a valid partner card
bool is_valid_partner_card(const sheepshead::interface::Hand& hand,
                           const sheepshead::interface::Card& card)
{
  if(!hand.rules().partner_by_called_ace()) return false;
  
  auto picker = *hand.history().picking_round().picker();  
  if(picker.is_null()) return false;
  auto picker_seat = hand.seat(picker);
  
  using sheepshead::interface::Card;
  
  // The picker can't call a partner card that she has. 
  if(std::find(picker_seat.held_cards_begin(),
               picker_seat.held_cards_end(),
               card)
      != picker_seat.held_cards_end()) {
    return false;
  }

  int ace_count = std::count_if(picker_seat.held_cards_begin(),
                                picker_seat.held_cards_end(),
              [](Card c)
              {return c.rank() == Card::Rank::ACE &&
                                  !c.is_trump();});

  int ten_count = std::count_if(picker_seat.held_cards_begin(),
                                 picker_seat.held_cards_end(),
              [](Card c)
              {return c.rank() == Card::Rank::TEN &&
                                  !c.is_trump();});
  
  std::set<Card::Suit> ace_suits; 
  std::set<Card::Suit> offsuits; 

  for(auto card_itr = picker_seat.held_cards_begin();
           card_itr != picker_seat.held_cards_end();
           ++card_itr)
  {
    if(card_itr->is_trump()) continue;
    
    offsuits.insert(card_itr->suit());

    if(card_itr->rank() == Card::Rank::ACE) {
      ace_suits.insert(card_itr->suit());
    }
  }
  
  std::set<Card::Suit> offsuits_with_no_ace;
  for(auto offsuit : offsuits) {
    if(ace_suits.find(offsuit) == ace_suits.end()) {
      offsuits_with_no_ace.insert(offsuit);
    }
  }

  if(card.rank() == Card::Rank::ACE) { 
    // If it's an ace, then the picker either must have an offsuit of that or
    // no offsuits for which she does not also have the ace
    if(offsuits_with_no_ace.size() > 0) {
      if(offsuits_with_no_ace.find(card.suit()) ==
          offsuits_with_no_ace.end())
        return false;
    } else {
      if(ace_suits.find(card.suit()) != ace_suits.end())
       return false; 
    }  
  }

  // If the card is a ten, then the picker must have all the aces
  if(card.rank() == Card::Rank::TEN) {
    if(ace_count != 3) return false;
  }
    
  // If the card is a king, then the picker must have all the aces and tens
  if(card.rank() == Card::Rank::KING) {
    if(ace_count != 3) return false;
    if(ten_count != 3) return false;
  }
  
  return true; 
}

TEST(TestPlaymaker, TestPartnerCards)
{
  auto hand = sheepshead::interface::Hand();

  for(int i=0; i<50; ++i) {
    hand.arbiter().arbitrate();

    auto player_itr = hand.history().picking_round().leader();
    hand.playmaker(*player_itr).make_play(testplays::pick);
    hand.playmaker(*player_itr).make_play(testplays::get_partner);

    auto available_plays = hand.playmaker(*player_itr).available_plays();
    
    std::string debug_string = hand.seat(*player_itr).DebugString();
    EXPECT_GT(available_plays.size(), 0);
    EXPECT_EQ(available_plays[0].play_type(),
              sheepshead::interface::Play::PlayType::PARTNER);
    
    for(auto play : available_plays) {
      EXPECT_TRUE(is_valid_partner_card(hand, *play.partner_decision()))
        << debug_string;
    }
    
    hand = sheepshead::interface::Hand();
  }
}

// Test the partner calls when there's only one offsuit available
TEST(TestPlaymaker, TestHasOneAvailableOffsuit)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();
  
  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds  {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::ACE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE)};
  hand.mock_blinds(mocked_blinds);
  
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::NINE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::EIGHT),
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
}

// Test partner calls when all partner aces are available
TEST(TestPlaymaker, TestHasNoAcesAllFail)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();
  
  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds  {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::TEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::TEN)};
  hand.mock_blinds(mocked_blinds);
  
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::HEARTS, Card::Rank::NINE),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::NINE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT)};
  
  hand.mock_held_cards(*player_itr, mocked_held_cards);
  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  auto available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 3);
  EXPECT_TRUE(std::all_of(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.play_type() == sheepshead::interface::Play::PlayType::PARTNER;}));
  EXPECT_TRUE(std::all_of(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.partner_decision()->rank() == sheepshead::interface::Card::Rank::ACE;}));

  hand.playmaker(*player_itr).make_play(available_plays[0]);
  available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_TRUE(std::all_of(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.play_type() == sheepshead::interface::Play::PlayType::DISCARD;}));
}

// Test partner calls when an unknown card must be declared
TEST(TestPlaymaker, TestAllTrumpTwoAces)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();
  
  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds  {
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::ACE)};
  hand.mock_blinds(mocked_blinds);
  
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::HEARTS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::QUEEN),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::JACK),
    std::make_pair(Card::Suit::SPADES, Card::Rank::JACK)};
  
  hand.mock_held_cards(*player_itr, mocked_held_cards);
  hand.playmaker(*player_itr).make_play(testplays::pick);
  EXPECT_EQ(hand.seat(*player_itr).number_of_held_cards(), 8);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  auto available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_EQ(available_plays.size(), 1);
  EXPECT_EQ(available_plays[0].play_type(),
            sheepshead::interface::Play::PlayType::PARTNER);
  EXPECT_EQ(available_plays[0].partner_decision()->suit(),
            sheepshead::interface::Card::Suit::HEARTS);
  EXPECT_EQ(available_plays[0].partner_decision()->rank(),
            sheepshead::interface::Card::Rank::ACE);

  // Make the partner play
  hand.playmaker(*player_itr).make_play(available_plays[0]);
  available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_GT(available_plays.size(), 0);
  EXPECT_TRUE(std::all_of(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.play_type() == sheepshead::interface::Play::PlayType::UNKNOWN;}));

  // Make the unknown play
  hand.playmaker(*player_itr).make_play(available_plays[0]);
  EXPECT_EQ(1, std::count_if(hand.seat(*player_itr).held_cards_begin(),
                             hand.seat(*player_itr).held_cards_end(),
        [](Card c){return c.is_unknown();}));
}

// Another unknown test
TEST(TestPlaymaker, TestNoFailAces)
{
  auto hand = testplays::TestHand();
  hand.arbiter().arbitrate();
  auto player_itr = hand.history().picking_round().leader();

  using sheepshead::interface::Card;
  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_blinds  {
    std::make_pair(Card::Suit::SPADES, Card::Rank::ACE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::TEN)};
  hand.mock_blinds(mocked_blinds);

  std::vector<std::pair<Card::Suit, Card::Rank> > mocked_held_cards {
    std::make_pair(Card::Suit::SPADES, Card::Rank::EIGHT),
    std::make_pair(Card::Suit::SPADES, Card::Rank::NINE),
    std::make_pair(Card::Suit::SPADES, Card::Rank::SEVEN),
    std::make_pair(Card::Suit::SPADES, Card::Rank::KING),
    std::make_pair(Card::Suit::CLUBS, Card::Rank::JACK),
    std::make_pair(Card::Suit::DIAMONDS, Card::Rank::JACK)};

  hand.mock_held_cards(*player_itr, mocked_held_cards);
  hand.playmaker(*player_itr).make_play(testplays::pick);
  hand.playmaker(*player_itr).make_play(testplays::get_partner);

  auto available_plays = hand.playmaker(*player_itr).available_plays();
  // Two aces could be called with unknown here
  EXPECT_EQ(available_plays.size(), 2);
  EXPECT_TRUE(std::all_of(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.play_type() == sheepshead::interface::Play::PlayType::PARTNER;}));
  EXPECT_TRUE(std::all_of(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.partner_decision()->rank() == sheepshead::interface::Card::Rank::ACE;}));

  // Clubs and hearts, not spades bc we have the ace
  EXPECT_EQ(1, std::count_if(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.partner_decision()->suit() == sheepshead::interface::Card::Suit::CLUBS;}));
  EXPECT_EQ(1, std::count_if(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.partner_decision()->suit() == sheepshead::interface::Card::Suit::HEARTS;}));

  hand.playmaker(*player_itr).make_play(available_plays[0]);
  available_plays = hand.playmaker(*player_itr).available_plays();
  EXPECT_GT(available_plays.size(), 0);
  EXPECT_TRUE(std::all_of(available_plays.begin(), available_plays.end(),
        [](sheepshead::interface::Play p)
        {return p.play_type() == sheepshead::interface::Play::PlayType::UNKNOWN;}));

  // Make the unknown play
  hand.playmaker(*player_itr).make_play(available_plays[0]);
  EXPECT_EQ(1, std::count_if(hand.seat(*player_itr).held_cards_begin(),
                             hand.seat(*player_itr).held_cards_end(),
        [](Card c){return c.is_unknown();}));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto results = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return results;
}
