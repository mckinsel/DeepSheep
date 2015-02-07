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
    plays.emplace_back(Play::PlayType::PICK, PickDecision::PICK);
    plays.emplace_back(Play::PlayType::PICK, PickDecision::PASS);
    return plays;
  }

  return plays;
}

} // namespace interface
} // namespace sheepshead
