#include "playmaker.h"
#include <cassert>

namespace sheepshead {
namespace interface {

Play::Play(PlayType play_tag, model::PickingRound_PickingDecision decision)
  :  m_play_tag(play_tag), m_pick_decision(decision)
{
  assert(m_play_tag == PlayType::PICK);
}

Play::Play(PlayType play_tag, model::PickingRound_LonerDecision decision)
  :  m_play_tag(play_tag), m_loner_decision(decision)
{
  assert(m_play_tag == PlayType::LONER);
}

Play::Play(PlayType play_tag, model::Card decision)
  :  m_play_tag(play_tag), m_card_decision(decision)
{
  assert(m_play_tag == PlayType::PARTNER ||
         m_play_tag == PlayType::TRICK_CARD);
}

Play::~Play()
{
  // A model::Card is not trivially destructible, so if that's what we've got,
  // we have to call its destructor.
  if(m_play_tag == PlayType::PARTNER || m_play_tag == PlayType::TRICK_CARD) {
    m_card_decision.~Card();
  }
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
      return m_card_decision.suit() == rhs.m_card_decision.suit() &&
             m_card_decision.rank() == rhs.m_card_decision.rank();
      break;
  }
}

bool Play::operator!=(const Play& rhs) const
{
  return !(*this == rhs);
}

Playmaker::Playmaker(const MutableHandHandle& hand_ptr, const PlayerId& playerid)
  : m_hand_ptr(hand_ptr), m_playerid(playerid)
{}

std::vector<Play> Playmaker::available_plays() const
{
  std::vector<Play> plays;

  return plays;
}

} // namespace interface
} // namespace sheepshead
