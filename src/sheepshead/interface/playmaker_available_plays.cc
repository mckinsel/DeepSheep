#include "playmaker_available_plays.h"

#include <iostream>
#include <set>

#include "history.h"
#include "rules.h"

namespace sheepshead {
namespace interface {
namespace internal {

Seat get_picker_seat(ConstHandHandle hand_ptr)
{
  int last_picking_index = hand_ptr->picking_round().picking_decisions_size() - 1;
  if(hand_ptr->picking_round().picking_decisions(last_picking_index) !=
        model::PickingRound::PICK)
    return Seat();

  int picker_position = (last_picking_index -
                         hand_ptr->picking_round().leader_position())
                        % Rules(hand_ptr).number_of_players();

  auto picker_id = PlayerId(hand_ptr, picker_position);
  auto picker_seat = Seat(hand_ptr, picker_id);

  return picker_seat;
}

Seat get_trick_player_seat(ConstHandHandle hand_ptr)
{
  auto latest_trick = History(hand_ptr).latest_trick();

  auto player_itr = std::next(latest_trick.leader(),
                              latest_trick.number_of_laid_cards());
  auto player_seat = Seat(hand_ptr, *player_itr);

  return player_seat;
}

void assign_model_suit(model::Card* model_card, Card::Suit suit)
{
  switch(suit) {
    case Card::Suit::CLUBS : model_card->set_suit(model::CLUBS); break;
    case Card::Suit::SPADES : model_card->set_suit(model::SPADES); break;
    case Card::Suit::DIAMONDS : model_card->set_suit(model::DIAMONDS); break;
    case Card::Suit::HEARTS : model_card->set_suit(model::HEARTS); break;
    default: assert(!"Unexpected suit.");
  }
}

void assign_model_rank(model::Card* model_card, Card::Rank rank)
{
  switch(rank) {
    case Card::Rank::ACE : model_card->set_rank(model::ACE); break;
    case Card::Rank::TEN : model_card->set_rank(model::TEN); break;
    case Card::Rank::KING : model_card->set_rank(model::KING); break;
    case Card::Rank::QUEEN : model_card->set_rank(model::QUEEN); break;
    case Card::Rank::JACK : model_card->set_rank(model::JACK); break;
    case Card::Rank::NINE : model_card->set_rank(model::NINE); break;
    case Card::Rank::EIGHT : model_card->set_rank(model::EIGHT); break;
    case Card::Rank::SEVEN : model_card->set_rank(model::SEVEN); break;
    default: assert(!"Unexpected rank.");
  }
}

// Given a vector of suits and a rank, create a vector of partner cards used to
// create permitted partner plays
std::vector<Card> create_partner_cards(
            const ConstHandHandle hand_ptr,
            const std::vector<Card::Suit>& permitted_suits,
            Card::Rank rank)
{
  std::vector<Card> output;
  for(auto& suit : permitted_suits) {
    model::Card model_card;
    assign_model_rank(&model_card, rank);
    assign_model_suit(&model_card, suit);
    model_card.set_unknown(false);
    output.emplace_back(hand_ptr, model_card);
  }
  return output;
}

// Return the suits that can be called as partner suits given a rank, usually
// an ace, the cards held by the picker, and the non-trump suits, as specified
// by the rules
std::vector<Card::Suit> get_permitted_partner_suits(
            Card::Rank rank,
            const std::vector<Card>& held_cards,
            const std::vector<Card::Suit>& offsuits)
{
  std::vector<Card::Suit> permitted_suits;

  std::set<Card::Suit> rank_suits_we_have;
  for(auto& card : held_cards) {
    if(card.rank() == rank && !card.is_trump())
       rank_suits_we_have.insert(card.suit());
  }

  for(auto& suit : offsuits) {
    if(rank_suits_we_have.find(suit) == rank_suits_we_have.end()) {
      permitted_suits.push_back(suit);
    }
  }

  return permitted_suits;
}

std::vector<Card> get_permitted_partner_cards(ConstHandHandle hand_ptr)
{
  std::vector<Card> output;

  auto picker_seat = get_picker_seat(hand_ptr);

  std::vector<Card> held_cards(picker_seat.held_cards_begin(),
                               picker_seat.held_cards_end());
  // The most common case is that there's a suit for which we have a fail card
  // but not the ace
  std::set<Card::Suit> fail_suits_we_have;
  for(auto& card : held_cards) {
    if(!card.is_trump()) fail_suits_we_have.insert(card.suit());
  }

  std::set<Card::Suit> ace_suits_we_have;
  for(auto& card : held_cards) {
    if(card.rank() == Card::Rank::ACE && !card.is_trump())
       ace_suits_we_have.insert(card.suit());
  }

  std::vector<Card::Suit> permitted_suits;
  for(auto& suit : fail_suits_we_have) {
    if(ace_suits_we_have.find(suit) == ace_suits_we_have.end()) {
      permitted_suits.push_back(suit);
    }
  }

  if(permitted_suits.size() > 0) {
    output = create_partner_cards(hand_ptr, permitted_suits, Card::Rank::ACE);
    return output;
  }

  // So we're here, which means that there's no fail suit for which we don't
  // also have the ace. The next thing to try is a non-trump suit for which we
  // do not have the ace. If that's the case, then later we can call an unknown
  // card
  std::vector<Card::Suit> offsuits {Card::Suit::CLUBS, Card::Suit::SPADES,
                                    Card::Suit::HEARTS};
  if(Rules(hand_ptr).trump_is_clubs()) {
    offsuits[0] = Card::Suit::DIAMONDS;
  }

  permitted_suits = get_permitted_partner_suits(Card::Rank::ACE, held_cards,
                                                offsuits);

  if(permitted_suits.size() > 0) {
    output = create_partner_cards(hand_ptr, permitted_suits, Card::Rank::ACE);
    return output;
  }

  // So now this means that we have all three offsuit aces. That means
  // we can move on to calling an offsuit ten that we do not have as the
  // partner card.
  permitted_suits = get_permitted_partner_suits(Card::Rank::TEN, held_cards,
                                                offsuits);

  if(permitted_suits.size() > 0) {
    output = create_partner_cards(hand_ptr, permitted_suits, Card::Rank::TEN);
    return output;
  }

  // Reaching here means that we have all offsuit aces and tens. The next thing
  // to try is kings.
  permitted_suits = get_permitted_partner_suits(Card::Rank::KING, held_cards,
                                                offsuits);
  if(permitted_suits.size() > 0) {
    output = create_partner_cards(hand_ptr, permitted_suits, Card::Rank::KING);
    return output;
  }

  // No we're in a situation that can only happend with a four player game with
  // a partner.
  permitted_suits = get_permitted_partner_suits(Card::Rank::NINE, held_cards,
                                                offsuits);
  if(permitted_suits.size() > 0) {
    output = create_partner_cards(hand_ptr, permitted_suits, Card::Rank::NINE);
    return output;
  }

  permitted_suits = get_permitted_partner_suits(Card::Rank::EIGHT, held_cards,
                                                offsuits);
  if(permitted_suits.size() > 0) {
    output = create_partner_cards(hand_ptr, permitted_suits, Card::Rank::EIGHT);
    return output;
  }

  assert(!"Unable to determine permitted partner calls.");
}

bool partner_suit_permits_discard(const std::vector<Card>& candidate_discard,
                                  Card::Suit partner_suit,
                                  int hand_partner_suit_count)
{
  int discard_trump_count = std::count_if(
      candidate_discard.begin(), candidate_discard.end(),
      [partner_suit](Card c){return c.suit() == partner_suit;});

  return discard_trump_count < hand_partner_suit_count;
}

std::vector<std::vector<Card> > get_permitted_discards(ConstHandHandle hand_ptr)
{
  
  std::vector<std::vector<Card>> output;

  auto picker_seat = get_picker_seat(hand_ptr);
  std::vector<Card> held_cards(picker_seat.held_cards_begin(),
                               picker_seat.held_cards_end());
 
  int number_of_cards_to_discard = Rules(hand_ptr).number_of_cards_in_blinds();
  std::vector<bool> selection_vector(held_cards.size(), false);
  std::fill(selection_vector.begin(),
            selection_vector.begin() + number_of_cards_to_discard,
            true);
  
  // If we don't have to worry about holding on to the partner suit, then we
  // can discard anything 
  bool constrained_by_partner_suit =
        Rules(hand_ptr).partner_is_allowed() &&
        Rules(hand_ptr).partner_by_called_ace() &&
        History(hand_ptr).picking_round().loner_decision() ==
                            LonerDecision::PARTNER;
  int partner_suit_count = 0;
  Card::Suit partner_suit = Card::Suit::UNKNOWN;

  if(constrained_by_partner_suit) {
    partner_suit = History(hand_ptr).picking_round().partner_card().suit();
    partner_suit_count = std::count_if(held_cards.begin(), held_cards.end(),
        [partner_suit](Card c){return c.suit() == partner_suit;});
  }

  // This is kind of terrible. For a 5 player hand, there are C(8,2) = 28
  // possible discards. For a 4 player hand, there are C(14,4) = 1001. Yikes.
  do {
    std::vector<Card> candidate_discard;
    candidate_discard.reserve(number_of_cards_to_discard);
    int card_index = 0;
    for(bool do_include : selection_vector) {
      if(do_include) candidate_discard.push_back(held_cards[card_index]);
      ++card_index;
    }
    
    if(!constrained_by_partner_suit) {  
        output.push_back(candidate_discard);
    } else {
      if(partner_suit_permits_discard(candidate_discard, partner_suit,
                                      partner_suit_count)) {
        output.push_back(candidate_discard);
      }
    }
  } while(std::prev_permutation(selection_vector.begin(), selection_vector.end()));

  return output;
}

std::vector<Card> get_permitted_trick_plays(ConstHandHandle hand_ptr)
{
  auto latest_trick = History(hand_ptr).latest_trick();

  auto player_itr = std::next(latest_trick.leader(),
                              latest_trick.number_of_laid_cards());
  auto player_seat = Seat(hand_ptr, *player_itr);

  // To start, we'll assume all cards are permitted and reduce that as we go on
  std::vector<Card> permitted_cards(player_seat.held_cards_begin(),
                                    player_seat.held_cards_end());

  // If we only have one card, then we can play it
  if(permitted_cards.size() == 1) {
    return permitted_cards;
  }

  // If we didn't lead, then we have to follow suit if we can
  if(player_itr != latest_trick.leader()) {
    auto led_suit = latest_trick.laid_cards_begin()->suit();
    int suit_following_card_count =
      std::count_if(permitted_cards.begin(), permitted_cards.end(),
                    [led_suit](const Card& c){return c.suit() == led_suit;});

    if(suit_following_card_count > 0) {
      permitted_cards.erase(std::remove_if(permitted_cards.begin(), permitted_cards.end(),
            [led_suit](const Card& c){return c.suit() != led_suit;}),
          permitted_cards.end());
    }
  }

  // There some extra rules for partners and pickers
  if(Rules(hand_ptr).partner_by_called_ace()) {

    auto partner_card = History(hand_ptr).picking_round().partner_card();

    // The picker cannot fail off all partner suit cards before the partner suit
    // has been led
    if(History(hand_ptr).picking_round().picker() == player_itr) {

      bool partner_card_already_played = std::any_of(
         History(hand_ptr).tricks_begin(), History(hand_ptr).tricks_end(),
           [&partner_card](const Trick<ConstHandHandle>& t)
           {
             return std::any_of(t.laid_cards_begin(),
                               t.laid_cards_end(),
                               [&partner_card](const Card& c){return c == partner_card;});
           });

      auto partner_suit_card_count = std::count_if(player_seat.held_cards_begin(),
                                                   player_seat.held_cards_end(),
            [&partner_card](const Card& c){return c.suit() == partner_card.suit();});

      // If there is only one partner suit card in hand, then we can't play it
      if(!partner_card_already_played && partner_suit_card_count < 2) {
        permitted_cards.erase(std::remove_if(permitted_cards.begin(), permitted_cards.end(),
              [&partner_card](Card& c){return c == partner_card;}), permitted_cards.end());
      }
    }

    // The partner must play the partner card when the partner suit is led
    if(std::any_of(player_seat.held_cards_begin(), player_seat.held_cards_end(),
         [&partner_card](const Card& c){return c == partner_card;})) {

      // If the partner suit was led, partner must play the partner card
      if(player_itr != latest_trick.leader()) {
        auto led_suit = latest_trick.laid_cards_begin()->suit();
        if(led_suit == partner_card.suit()) {
          permitted_cards.clear();
          permitted_cards.push_back(partner_card);
        }
      } else {
      // If the partner is leading, she can't lead a card that's the partner
      // suit but isn't the partner card
        permitted_cards.erase(std::remove_if(permitted_cards.begin(), permitted_cards.end(),
            [&partner_card](const Card& c){return c.suit() == partner_card.suit() &&
                                                  c != partner_card;}),
            permitted_cards.end());
      }
    }
  }
  return permitted_cards;
}

} // namespace internal
} // namespace interface
} // namespace sheepshead
