#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYMAKER_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYMAKER_H_

#include "sheepshead/proto/game.pb.h"
#include "sheepshead/interface/handle_types.h"
#include "sheepshead/interface/playerid.h"
#include "sheepshead/interface/deck.h"
#include "sheepshead/interface/pickinground.h"

#include <utility>
#include <vector>

namespace sheepshead {
namespace interface {


/// Represents a single possible play for a Hand.
class Play
{
public:
  // Enum for the different types of play that a player can make.
  enum class PlayType {PICK, LONER, PARTNER, UNKNOWN, DISCARD, TRICK_CARD};

  // Constructors for the different Play types.
  // PICK
  Play(PlayType play_tag, PickDecision decision);
  // LONER
  Play(PlayType play_tag, LonerDecision decision);
  // PARTNER, TRICK_CARD
  Play(PlayType play_tag, Card decision);
  // UNKNOWN
  Play(PlayType play_tag, std::pair<Card, Card::Suit> decision);
  // DISCARD
  Play(PlayType play_tag, std::vector<Card> decision);

  // These all need to be explicit because of the variant type.
  virtual ~Play();
  Play(const Play& from);
  Play& operator=(const Play& from);

  bool operator==(const Play& rhs) const;
  bool operator!=(const Play& rhs) const;

  // Accessors for play type and the play itself.
  PlayType play_type() const;
  const PickDecision* pick_decision() const;
  const LonerDecision* loner_decision() const;
  const Card* partner_decision() const;
  const Card* trick_card_decision() const;
  const std::pair<Card, Card::Suit>* unknown_decision() const;
  const std::vector<Card>* discard_decision() const;

  std::string debug_string() const;

private:

  PlayType m_play_tag;
  union {
    PickDecision m_pick_decision;
    LonerDecision m_loner_decision;
    Card m_card_decision;
    std::pair<Card, Card::Suit> m_unknown_decision;
    std::vector<Card> m_discard_decision;
  };

}; // class Play

/// The interface to altering Hand by having a player make plays.
class Playmaker
{
public:
  bool make_play(const Play& play);
  std::vector<Play> available_plays() const;

private:
  friend class Hand;
  Playmaker(const MutableHandHandle& hand_ptr, const PlayerId& playerid);
  MutableHandHandle m_hand_ptr;
  PlayerId m_playerid;

}; // class Playmaker

} // namespace interface
} // namespace sheepshead
#endif
