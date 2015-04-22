#include "playmaker.h"

#include "handstate.h"
#include "pickinground.h"
#include "playmaker_available_plays.h"
#include "seat.h"

#include <algorithm>
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
      new (&m_card_decision) Card(from.m_card_decision);
      break;
    case PlayType::TRICK_CARD :
      new (&m_card_decision) Card(from.m_card_decision);
      m_card_decision = from.m_card_decision;
      break;
    case PlayType::DISCARD :
      new (&m_discard_decision) std::vector<Card>();
      m_discard_decision = from.m_discard_decision;
      break;
    case PlayType::UNKNOWN :
      new (&m_unknown_decision) std::pair<Card, Card::Suit>();
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
  return false;
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

std::string Play::debug_string() const
{
  std::string output_string("Play type is ");

  switch(m_play_tag) {

    case Play::PlayType::PICK :
      output_string.append("PICK:\n");
      switch(m_pick_decision) {
        case PickDecision::PICK :
          output_string.append("PICK\n");
          break;
        case PickDecision::PASS :
          output_string.append("PASS\n");
          break;
        case PickDecision::UNASKED :
          output_string.append("UNASKED\n");
          break;
      }
      break;

    case Play::PlayType::LONER :
      output_string.append("LONER:\n");
      switch(m_loner_decision) {
        case LonerDecision::LONER :
          output_string.append("LONER\n");
          break;
        case LonerDecision::PARTNER :
          output_string.append("PARTNER\n");
          break;
        case LonerDecision::NONE :
          output_string.append("NONE\n");
          break;
      }
      break;

    case Play::PlayType::PARTNER :
      output_string.append("PARTNER:\n");
      output_string.append(m_card_decision.debug_string());
      break;

    case Play::PlayType::UNKNOWN :
      output_string.append("UNKNOWN:\n");
      output_string.append(m_unknown_decision.first.debug_string());
      break;

    case Play::PlayType::DISCARD :
      output_string.append("DISCARD:\n");
      for(auto& discard : m_discard_decision) {
        output_string.append(discard.debug_string());
      }
      break;

    case Play::PlayType::TRICK_CARD :
      output_string.append("TRICK_CARD\n");
      output_string.append(m_card_decision.debug_string());
      break;
  }

  return output_string;
}

// end Play

// Playmaker

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
      auto picker_seat = internal::get_picker_seat(m_hand_ptr);
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
    auto cards = internal::get_permitted_partner_cards(m_hand_ptr);
    for(auto card : cards) {
      plays.emplace_back(Play::PlayType::PARTNER, card);
    }
    return plays;
  }

  // The unknown card decision
  if(internal::ready_for_unknown_play(m_hand_ptr) == m_playerid) {

    // Permitted unknown cards are just any card in the picker's hand I think
    auto picker_seat = internal::get_picker_seat(m_hand_ptr);
    auto model_partner_card = m_hand_ptr->picking_round().partner_card();
    auto partner_card = Card(m_hand_ptr, model_partner_card);
    for(auto card_itr=picker_seat.held_cards_begin();
             card_itr!=picker_seat.held_cards_end();
             ++card_itr)
    {
      plays.emplace_back(Play::PlayType::UNKNOWN,
                         std::make_pair(*card_itr, partner_card.suit()));
    }
    return plays;
  }

  // The discard decision
  if(internal::ready_for_discard_play(m_hand_ptr) == m_playerid) {
    auto discard_vectors = internal::get_permitted_discards(m_hand_ptr);
    for(auto& discard_vector : discard_vectors) {
      plays.emplace_back(Play::PlayType::DISCARD, discard_vector);
    }
    return plays;
  }

  // A trick card decision
  if(internal::ready_for_trick_play(m_hand_ptr) == m_playerid) {
    auto trick_cards = internal::get_permitted_trick_plays(m_hand_ptr);
    for(auto& trick_card : trick_cards) {
      plays.emplace_back(Play::PlayType::TRICK_CARD, trick_card);
    }
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
  internal::assign_model_suit(model_card, partner_card->true_suit());
  internal::assign_model_rank(model_card, partner_card->true_rank());

  // Now for the trickiness. If the partner card is the ace of suit for which
  // the picker has no fail cards, then proceed to the unknown step. Otherwise,
  // we can go the discard step.
  hand_ptr->mutable_picking_round()->set_unknown_decision_made(false);
  if(partner_card->rank() == Card::Rank::ACE) {
    auto partner_suit = partner_card->suit();
    auto picker_seat = internal::get_picker_seat(hand_ptr);

    bool picker_has_partner_suit_fail =
      std::any_of(picker_seat.held_cards_begin(), picker_seat.held_cards_end(),
          [partner_suit](Card card){return card.suit() == partner_suit;});

    if(picker_has_partner_suit_fail) {
      hand_ptr->mutable_picking_round()->set_unknown_decision_made(true);
    }
  } else {
    hand_ptr->mutable_picking_round()->set_unknown_decision_made(false);
  }
  return true;
}

bool make_unknown_play(MutableHandHandle hand_ptr, const Play& play)
{
  auto unknown_pair = play.unknown_decision();
  auto unknown_card = model::Card();
  internal::assign_model_suit(&unknown_card, unknown_pair->first.true_suit());
  internal::assign_model_rank(&unknown_card, unknown_pair->first.true_rank());

  int picker_position = (hand_ptr->picking_round().picking_decisions_size() -
                          hand_ptr->picking_round().leader_position() - 1) %
                        Rules(hand_ptr).number_of_players();

  auto picker_seat = hand_ptr->mutable_seats(picker_position);
  for(auto& model_card : *picker_seat->mutable_held_cards()) {
    if(model_card.suit() == unknown_card.suit() &&
       model_card.rank() == unknown_card.rank()) {
      model_card.set_unknown(true);
    }
  }
  hand_ptr->mutable_picking_round()->set_unknown_decision_made(true);
  return true;
}

bool make_discard_play(MutableHandHandle hand_ptr, const Play& play)
{
  auto discards = play.discard_decision();

  // Create model::Cards for each card to be discarded
  std::vector<model::Card> model_discards;
  for(auto& discard : *discards) {
    auto model_discard = model::Card();
    internal::assign_model_suit(&model_discard, discard.true_suit());
    internal::assign_model_rank(&model_discard, discard.true_rank());
    model_discards.push_back(model_discard);
  }

  int picker_position = (hand_ptr->picking_round().picking_decisions_size() -
                          hand_ptr->picking_round().leader_position() - 1) %
                        Rules(hand_ptr).number_of_players();

  auto held_cards = hand_ptr->seats(picker_position).held_cards();
  std::vector<model::Card> new_held_cards;

  for(auto& held_card : held_cards) {
    if(std::any_of(model_discards.begin(), model_discards.end(),
           [&held_card](model::Card discard)
           {return held_card.suit() == discard.suit() &&
                   held_card.rank() == discard.rank();})) {
      auto new_discard = hand_ptr->mutable_picking_round()->add_discarded_cards();
      *new_discard = held_card;
    } else {
      new_held_cards.push_back(held_card);
    }
  }

  hand_ptr->mutable_seats(picker_position)->clear_held_cards();
  for(auto new_held_card : new_held_cards) {
    auto new_card = hand_ptr->mutable_seats(picker_position)->add_held_cards();
    *new_card = new_held_card;
  }

 return true;
}

bool make_trick_card_play(MutableHandHandle hand_ptr, const Play& play)
{
  auto last_model_trick = hand_ptr->mutable_tricks(hand_ptr->tricks_size() - 1);
  auto player_position = (last_model_trick->laid_cards_size() +
                          last_model_trick->leader_position()) %
                         Rules(hand_ptr).number_of_players();
  auto new_trick_card = last_model_trick->add_laid_cards();

  auto trick_card = play.trick_card_decision();

  internal::assign_model_suit(new_trick_card, trick_card->true_suit());
  internal::assign_model_rank(new_trick_card, trick_card->true_rank());
  if(trick_card->is_unknown()) {
    new_trick_card->set_unknown(true);
  }

  auto held_cards = hand_ptr->seats(player_position).held_cards();
  std::vector<model::Card> new_held_cards;
  for(auto& held_card : held_cards) {
    if(held_card.suit() != new_trick_card->suit() ||
       held_card.rank() != new_trick_card->rank()) {
      new_held_cards.push_back(held_card);
    }
  }

  assert(held_cards.size() - new_held_cards.size() == 1);

  hand_ptr->mutable_seats(player_position)->clear_held_cards();
  for(auto new_held_card : new_held_cards) {
    auto new_card = hand_ptr->mutable_seats(player_position)->add_held_cards();
    *new_card = new_held_card;
  }

  return true;
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
      return make_unknown_play(m_hand_ptr, play);
      break;

    case Play::PlayType::DISCARD :
      ready_player = internal::ready_for_discard_play(m_hand_ptr);
      if(ready_player != m_playerid)
        return false;
      return make_discard_play(m_hand_ptr, play);
      break;

    case Play::PlayType::TRICK_CARD :
      ready_player = internal::ready_for_trick_play(m_hand_ptr);
      if(ready_player != m_playerid)
        return false;
      return make_trick_card_play(m_hand_ptr, play);
      break;

    default:
      assert(!"Oh no, a mystery play");
  }
  return false;
}

} // namespace interface
} // namespace sheepshead
