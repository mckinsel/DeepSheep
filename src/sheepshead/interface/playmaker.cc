#include "playmaker.h"

#include "handstate.h"

#include <cassert>

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

Play::Play(PlayType play_tag, Card&& decision)
  :  m_play_tag(play_tag), m_card_decision(decision)
{
  assert(m_play_tag == PlayType::PARTNER ||
         m_play_tag == PlayType::TRICK_CARD);
}

Play::Play(PlayType play_tag, std::pair<Card, Card::Suit>&& decision)
  : m_play_tag(play_tag), m_unknown_decision(decision)
{
  assert(m_play_tag == PlayType::UNKNOWN);
}

Play::Play(PlayType play_tag, std::vector<Card>&& decision)
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

Playmaker::Playmaker(const MutableHandHandle& hand_ptr, const PlayerId& playerid)
  : m_hand_ptr(hand_ptr), m_playerid(playerid)
{}

std::vector<Play> Playmaker::available_plays() const
{
  std::vector<Play> plays;

  // If the hand cannot be played, just return an empty vector.
  if(!internal::is_playable(m_hand_ptr)) return plays;

  if(internal::ready_for_pick_play(m_hand_ptr) == m_playerid) {
    // Hand the last picker forced pick rule
    if(*std::prev(History(m_hand_ptr).picking_round().leader()) == m_playerid &&
       Rules(m_hand_ptr).no_picker_forced_pick()) {
      plays.emplace_back(Play::PlayType::PICK, PickDecision::PICK);
    } else {
      plays.emplace_back(Play::PlayType::PICK, PickDecision::PICK);
      plays.emplace_back(Play::PlayType::PICK, PickDecision::PASS);
  }
    return plays;
  }

  if(internal::ready_for_loner_play(m_hand_ptr) == m_playerid) {
    plays.emplace_back(Play::PlayType::LONER, LonerDecision::LONER);
    plays.emplace_back(Play::PlayType::LONER, LonerDecision::PARTNER);
    return plays;
  }

  return plays;
}

bool make_pick_play(MutableHandHandle hand_ptr, const Play& play)
{
  if(*play.pick_decision() == PickDecision::PICK) {
    hand_ptr->mutable_picking_round()->add_picking_decisions(model::PickingRound::PICK);
  } else if(*play.pick_decision() == PickDecision::PASS) {
    hand_ptr->mutable_picking_round()->add_picking_decisions(model::PickingRound::PASS);
  } else {
    return false;
  }
  return true;
}

bool make_loner_play(MutableHandHandle hand_ptr, const Play& play)
{
  if(*play.loner_decision() == LonerDecision::LONER) {
    hand_ptr->mutable_picking_round()->set_loner_decision(model::PickingRound::LONER);
  } else if(*play.loner_decision() == LonerDecision::PARTNER) {
    hand_ptr->mutable_picking_round()->set_loner_decision(model::PickingRound::PARTNER);
  } else {
    return false;
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
