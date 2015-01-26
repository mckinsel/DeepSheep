#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYMAKER_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYMAKER_H_

#include "sheepshead/proto/game.pb.h"
#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/playerid.h"

#include <vector>

namespace sheepshead {
namespace interface {

/// Enum for the different types of play that a player can make.

/// Represents a single possible play for a Hand.
class Play
{
public:
  // Explicit destructor to handle destruction of variant type.
  virtual ~Play();

  bool operator==(const Play& rhs) const;
  bool operator!=(const Play& rhs) const;

private:
  enum class PlayType {PICK, LONER, PARTNER, TRICK_CARD};

  Play(PlayType play_tag, model::PickingRound_PickingDecision decision);
  Play(PlayType play_tag, model::PickingRound_LonerDecision decision);
  Play(PlayType play_tag, model::Card decision);
  //Play(PlayTupe play_tag, std::tuple<model::Card, model::Suit> decision);
  //Play(PlayType play_tag, std::vector<model::Card> decision);

  const PlayType m_play_tag;
  const union {
    model::PickingRound_PickingDecision m_pick_decision;
    model::PickingRound_LonerDecision m_loner_decision;
    model::Card m_card_decision;
  };

}; // class Play

class Playmaker
{
public:
  void make_play(const Play& play);
  std::vector<Play> available_plays() const;

private:
  friend class Hand;
  Playmaker(const MutableHandHandle& hand_ptr, const PlayerId& playerid);
  const MutableHandHandle m_hand_ptr;
  const PlayerId m_playerid;

}; // class Playmaker

} // namespace interface
} // namespace sheepshead
#endif
