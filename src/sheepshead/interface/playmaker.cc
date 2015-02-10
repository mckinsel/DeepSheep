#include "playmaker.h"

#include "handstate.h"
#include "seat.h"
#include "pickinground.h"

#include <cassert>
#include <iostream>
#include <set>

namespace sheepshead {
namespace interface {

// Play
Play::Play(PlayType play_tag, PickDecision decision)
  :  m_play_tag(play_tag), m_pick_decision(decision)
{
  assert(m_play_tag == PlayType::PICK);
}

Play::Play(PlayType play_tag, LonerDecision decision)
  :  m_play_tag(play_tag), m_loner_decision(decision)
{
  assert(m_play_tag == PlayType::LONER);
}

Play::Play(PlayType play_tag, Card decision)
  :  m_play_tag(play_tag), m_card_decision(decision)
{
  assert(m_play_tag == PlayType::PARTNER ||
         m_play_tag == PlayType::TRICK_CARD);
}

Play::Play(PlayType play_tag, std::pair<Card, Card::Suit> decision)
  : m_play_tag(play_tag), m_unknown_decision(decision)
{
  assert(m_play_tag == PlayType::UNKNOWN);
}

Play::Play(PlayType play_tag, std::vector<Card> decision)
  : m_play_tag(play_tag), m_discard_decision(decision)
{
  assert(m_play_tag == PlayType::DISCARD);
}

Play::~Play()
{
  // A model::Card is not trivially destructible, so if that's what we've got,
  // we have to call its destructor.
  if(m_play_tag == PlayType::UNKNOWN) {
    m_unknown_decision.~pair<Card, Card::Suit>();
  } else if(m_play_tag == PlayType::DISCARD) {
    m_discard_decision.~vector<Card>();
  } else if(m_play_tag == PlayType::PARTNER ||
            m_play_tag == PlayType::TRICK_CARD) {
    m_card_decision.~Card();
  }
}

Play::Play(const Play& from)
: m_play_tag(from.m_play_tag)
{
  switch(m_play_tag) {
    case PlayType::PICK :
      m_pick_decision = from.m_pick_decision;
      break;
    case PlayType::LONER :
      m_loner_decision = from.m_loner_decision;
      break;
    case PlayType::PARTNER :
    case PlayType::TRICK_CARD :
      m_card_decision = from.m_card_decision;
      break;
    case PlayType::DISCARD :
      m_discard_decision = from.m_discard_decision;
      break;
    case PlayType::UNKNOWN :
      m_unknown_decision = from.m_unknown_decision;
      break;
  }
}

Play& Play::operator=(const Play& from)
{
  m_play_tag = from.m_play_tag;
  switch(m_play_tag) {
    case PlayType::PICK :
      m_pick_decision = from.m_pick_decision;
      break;
    case PlayType::LONER :
      m_loner_decision = from.m_loner_decision;
      break;
    case PlayType::PARTNER :
    case PlayType::TRICK_CARD :
      m_card_decision = from.m_card_decision;
      break;
    case PlayType::DISCARD :
      m_discard_decision = from.m_discard_decision;
      break;
    case PlayType::UNKNOWN :
      m_unknown_decision = from.m_unknown_decision;
      break;
  }

  return *this;
}

bool Play::operator==(const Play& rhs) const
{
  if(m_play_tag != rhs.m_play_tag)
    return false;

  switch(m_play_tag) {
    case PlayType::PICK :
      return m_pick_decision == rhs.m_pick_decision;
      break;
    case PlayType::LONER :
      return m_loner_decision == rhs.m_loner_decision;
      break;
    case PlayType::PARTNER :
    case PlayType::TRICK_CARD :
      return m_card_decision == rhs.m_card_decision;
      break;
    case PlayType::DISCARD :
      if(m_discard_decision.size() !=
          rhs.m_discard_decision.size())
        return false;
      for(size_t i=0; i < m_discard_decision.size(); i++) {
        if(m_discard_decision[i] != rhs.m_discard_decision[i])
          return false;
      }
      return true;
      break;
    case PlayType::UNKNOWN :
      return m_unknown_decision == rhs.m_unknown_decision;
      break;
  }
}

bool Play::operator!=(const Play& rhs) const
{
  return !(*this == rhs);
}

Play::PlayType Play::play_type() const
{
  return m_play_tag;
}

const PickDecision* Play::pick_decision() const
{
  if(m_play_tag != PlayType::PICK) return nullptr;
  return &m_pick_decision;
}

const LonerDecision* Play::loner_decision() const
{
  if(m_play_tag != PlayType::LONER) return nullptr;
  return &m_loner_decision;
}

const Card* Play::partner_decision() const
{
  if(m_play_tag != PlayType::PARTNER) return nullptr;
  return &m_card_decision;
}

const Card* Play::trick_card_decision() const
{
  if(m_play_tag != PlayType::TRICK_CARD) return nullptr;
  return &m_card_decision;
}

const std::pair<Card, Card::Suit>* Play::unknown_decision() const
{
  if(m_play_tag != PlayType::UNKNOWN) return nullptr;
  return &m_unknown_decision;
}

const std::vector<Card>* Play::discard_decision() const
{
  if(m_play_tag != PlayType::DISCARD) return nullptr;
  return &m_discard_decision;
}

// end Play

// Playmaker

// Return the Seat for the player who picked.
Seat _get_picker_seat(ConstHandHandle hand_ptr)
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

void _assign_model_suit(model::Card* model_card, Card::Suit suit)
{
  switch(suit) {
    case Card::Suit::CLUBS : model_card->set_suit(model::CLUBS); break;
    case Card::Suit::SPADES : model_card->set_suit(model::SPADES); break;
    case Card::Suit::DIAMONDS : model_card->set_suit(model::DIAMONDS); break;
    case Card::Suit::HEARTS : model_card->set_suit(model::HEARTS); break;
    default: assert(!"Unexpected suit.");
  }
}

void _assign_model_rank(model::Card* model_card, Card::Rank rank)
{
  switch(rank) {
    case Card::Rank::ACE : model_card->set_rank(model::ACE); break;
    case Card::Rank::TEN : model_card->set_rank(model::TEN); break;
    case Card::Rank::KING : model_card->set_rank(model::KING); break;
    case Card::Rank::QUEEN : model_card->set_rank(model::QUEEN); break;
    case Card::Rank::JACK : model_card->set_rank(model::JACK); break;
    case Card::Rank::NINE : model_card->set_rank(model::NINE); break;
    case Card::Rank::EIGHT : model_card->set_rank(model::EIGHT); break;
    case Card::Rank::SEVEN : model_card->set_rank(model::EIGHT); break;
    default: assert(!"Unexpected rank.");
  }

}

// Given a vector of suits and a rank, fill up the plays_ptr with the implied
// partner cards.
std::vector<Card> _create_partner_cards(
        const ConstHandHandle hand_ptr,
        const std::vector<Card::Suit>& permitted_suits,
        Card::Rank rank)
{
  std::vector<Card> output;
  for(auto& suit : permitted_suits) {
    model::Card model_card;
    _assign_model_rank(&model_card, rank);
    _assign_model_suit(&model_card, suit);
    model_card.set_unknown(false);
    output.emplace_back(hand_ptr, model_card);
  }
  return output;
}

// A helper function for get_available_partner_cards. Gets suits that can be
// called given a rank, the cards held by the picker, an the offsuits of the
// hand.
std::vector<Card::Suit> _get_permitted_suits(
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

// Return a vector of Cards that the picker could call as partner cards.
std::vector<Card> get_available_partner_cards(ConstHandHandle hand_ptr)
{
  std::vector<Card> output;

  auto picker_seat = _get_picker_seat(hand_ptr);

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
    output = _create_partner_cards(hand_ptr, permitted_suits, Card::Rank::ACE);
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

  permitted_suits = _get_permitted_suits(Card::Rank::ACE, held_cards,
                                         offsuits);

  if(permitted_suits.size() > 0) {
    output = _create_partner_cards(hand_ptr, permitted_suits, Card::Rank::ACE);
    return output;
  }

  // So now this means that we have all three offsuit aces. That means
  // we can move on to calling an offsuit ten that we do not have as the
  // partner card.
  permitted_suits = _get_permitted_suits(Card::Rank::TEN, held_cards,
                                         offsuits);

  if(permitted_suits.size() > 0) {
    output = _create_partner_cards(hand_ptr, permitted_suits, Card::Rank::TEN);
    return output;
  }

  // Reaching here means that we have all offsuit aces and tens. The next thing
  // to try is kings.
  permitted_suits = _get_permitted_suits(Card::Rank::KING, held_cards,
                                         offsuits);
  if(permitted_suits.size() > 0) {
    output = _create_partner_cards(hand_ptr, permitted_suits, Card::Rank::KING);
    return output;
  }

  // No we're in a situation that can only happend with a four player game with
  // a partner.
  permitted_suits = _get_permitted_suits(Card::Rank::NINE, held_cards,
                                         offsuits);
  if(permitted_suits.size() > 0) {
    output = _create_partner_cards(hand_ptr, permitted_suits, Card::Rank::NINE);
    return output;
  }

  permitted_suits = _get_permitted_suits(Card::Rank::EIGHT, held_cards,
                                         offsuits);
  if(permitted_suits.size() > 0) {
    output = _create_partner_cards(hand_ptr, permitted_suits, Card::Rank::EIGHT);
    return output;
  }

  assert(!"Unable to determine permitted partner calls.");
}

Playmaker::Playmaker(const MutableHandHandle& hand_ptr, const PlayerId& playerid)
  : m_hand_ptr(hand_ptr), m_playerid(playerid)
{}

std::vector<Play> Playmaker::available_plays() const
{
  std::vector<Play> plays;

  // If the hand cannot be played, just return an empty vector.
  if(!internal::is_playable(m_hand_ptr)) return plays;

  // The pick decision
  if(internal::ready_for_pick_play(m_hand_ptr) == m_playerid) {
    // Handle the last picker forced pick rule
    if(*std::prev(History(m_hand_ptr).picking_round().leader()) == m_playerid &&
       Rules(m_hand_ptr).no_picker_forced_pick()) {
      plays.emplace_back(Play::PlayType::PICK, PickDecision::PICK);
    } else {
      plays.emplace_back(Play::PlayType::PICK, PickDecision::PICK);
      plays.emplace_back(Play::PlayType::PICK, PickDecision::PASS);
  }
    return plays;
  }

  // The loner decision
  if(internal::ready_for_loner_play(m_hand_ptr) == m_playerid) {
    plays.emplace_back(Play::PlayType::LONER, LonerDecision::LONER);

    // To know if a partner call is available, we have to make sure that it
    // isn't the case the jack of diamonds is partner and the picker has it.
    if(!Rules(m_hand_ptr).partner_by_jack_of_diamonds()) {
      plays.emplace_back(Play::PlayType::LONER, LonerDecision::PARTNER);
    } else {
      auto picker_seat = _get_picker_seat(m_hand_ptr);
      if(!std::any_of(picker_seat.held_cards_begin(), picker_seat.held_cards_end(),
            [](Card card){return card.true_suit() == Card::Suit::DIAMONDS &&
                                 card.true_rank() == Card::Rank::JACK;})) {
      plays.emplace_back(Play::PlayType::LONER, LonerDecision::PARTNER);
      }
    }
    return plays;
  }

  // The called partner decision
  if(internal::ready_for_partner_play(m_hand_ptr) == m_playerid) {
    auto cards = get_available_partner_cards(m_hand_ptr);
    std::cout << "Found " << cards.size() << " partner cards." << std::endl;
    for(auto& card : cards) {
      plays.emplace_back(Play::PlayType::PARTNER, card);
    }
    return plays;
  }

  // The unknown card decision
  if(internal::ready_for_unknown_play(m_hand_ptr) == m_playerid) {
    // Permitted unknown cards are just any card in the picker's hand I think
    auto picker_seat = _get_picker_seat(m_hand_ptr);
    for(auto card_itr=picker_seat.held_cards_begin();
             card_itr!=picker_seat.held_cards_end();
             ++card_itr)
    {
      plays.emplace_back(Play::PlayType::UNKNOWN, *card_itr);
    }
    return plays;
  }

  // The discard decision
  if(internal::ready_for_discard_play(m_hand_ptr) == m_playerid) {
    // TODO: implement discard decision
    return plays;
  }

  return plays;
}

// Modify the hand with a pick play. Assumed to be made by the player eligible
// to pick.
bool make_pick_play(MutableHandHandle hand_ptr, const Play& play)
{
  if(*play.pick_decision() == PickDecision::PICK) {
    int picker_position = (hand_ptr->picking_round().picking_decisions_size() -
                           hand_ptr->picking_round().leader_position()) %
                          Rules(hand_ptr).number_of_players();
    auto picker_seat = hand_ptr->mutable_seats(picker_position);

    // When someone picks, they also pick up the blinds
    for(auto& model_card : hand_ptr->picking_round().blinds()) {
      auto new_card = picker_seat->add_held_cards();
      *new_card = model_card;
    }
    hand_ptr->mutable_picking_round()->clear_blinds();

    hand_ptr->mutable_picking_round()->
      add_picking_decisions(model::PickingRound::PICK);

    // If the rules say no partner, then the hand can proceed to the discard
    // step.
    if(!Rules(hand_ptr).partner_is_allowed()) {
      hand_ptr->mutable_picking_round()->
        set_loner_decision(model::PickingRound::LONER);
      hand_ptr->mutable_picking_round()->
        clear_partner_card();
      hand_ptr->mutable_picking_round()->
        set_unknown_decision_made(true);
    }

  // If the player passes, then it's easy. Just record the decision
  } else if(*play.pick_decision() == PickDecision::PASS) {
    hand_ptr->mutable_picking_round()->
      add_picking_decisions(model::PickingRound::PASS);

  } else {
    return false;
  }
  return true;
}

bool make_loner_play(MutableHandHandle hand_ptr, const Play& play)
{
  if(*play.loner_decision() == LonerDecision::LONER) {
    hand_ptr->mutable_picking_round()->
      set_loner_decision(model::PickingRound::LONER);
  } else if(*play.loner_decision() == LonerDecision::PARTNER) {
    hand_ptr->mutable_picking_round()->
      set_loner_decision(model::PickingRound::PARTNER);
  } else {
    return false;
  }

  // We may be able to advance to the discard stage: if the picker decided to
  // go alone, or he wanted a partner but parnter is by jack of diamonds
  if(*play.loner_decision() == LonerDecision::LONER ||
     Rules(hand_ptr).partner_by_jack_of_diamonds()) {
    hand_ptr->mutable_picking_round()->
      clear_partner_card();
    hand_ptr->mutable_picking_round()->
      set_unknown_decision_made(true);
  }
  return true;
}

bool make_partner_play(MutableHandHandle hand_ptr, const Play& play)
{
  auto partner_card = play.partner_decision();
  auto model_card = hand_ptr->mutable_picking_round()->mutable_partner_card();
  _assign_model_suit(model_card, partner_card->true_suit());
  _assign_model_rank(model_card, partner_card->true_rank());

  // Now for the trickiness. If the partner card is the ace of suit for which
  // the picker has no fail cards, then proceed to the unknown step. Otherwise,
  // we can go the discard step.
  hand_ptr->mutable_picking_round()->set_unknown_decision_made(false);
  if(partner_card->rank() == Card::Rank::ACE) {
    auto partner_suit = partner_card->suit();
    auto picker_seat = _get_picker_seat(hand_ptr);

    bool picker_has_partner_suit_fail =
      std::any_of(picker_seat.held_cards_begin(), picker_seat.held_cards_end(),
          [partner_suit](Card card){return card.suit() == partner_suit;});

    if(picker_has_partner_suit_fail) {
      hand_ptr->mutable_picking_round()->set_unknown_decision_made(true);
    }
  } else {
    hand_ptr->mutable_picking_round()->set_unknown_decision_made(true);
  }
}

bool Playmaker::make_play(const Play& play)
{
  PlayerId ready_player;

  switch(play.play_type()) {

    case Play::PlayType::PICK :
      ready_player = internal::ready_for_pick_play(m_hand_ptr);
      if(ready_player != m_playerid)
        return false;
      return make_pick_play(m_hand_ptr, play);
      break;

    case Play::PlayType::LONER :
      ready_player = internal::ready_for_loner_play(m_hand_ptr);
      if(ready_player != m_playerid)
        return false;
      return make_loner_play(m_hand_ptr, play);
      break;

    case Play::PlayType::PARTNER :
      ready_player = internal::ready_for_partner_play(m_hand_ptr);
      if(ready_player != m_playerid)
        return false;
      return make_partner_play(m_hand_ptr, play);
      break;

    case Play::PlayType::UNKNOWN :
      ready_player = internal::ready_for_unknown_play(m_hand_ptr);
      if(ready_player != m_playerid)
        return false;
      break;

    case Play::PlayType::DISCARD :
      ready_player = internal::ready_for_discard_play(m_hand_ptr);
      if(ready_player != m_playerid)
        return false;
      break;

    case Play::PlayType::TRICK_CARD :
      ready_player = internal::ready_for_trick_play(m_hand_ptr);
      if(ready_player != m_playerid)
        return false;
      break;

    default:
      assert(!"Oh no, a mystery play");
  }
  return false;
}

} // namespace interface
} // namespace sheepshead
