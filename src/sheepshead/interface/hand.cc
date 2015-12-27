#include "hand.h"
#include "handstate.h"
#include "playmaker_available_plays.h"

#include <algorithm>
#include <chrono>
#include <sstream>

#include <assert.h>

namespace sheepshead {
namespace interface {

Hand::Hand(unsigned long random_seed)
{
  m_hand_ptr = std::make_shared<sheepshead::model::Hand> ();
  m_hand_ptr->mutable_rule_variation();
  if(random_seed == 0) {
    m_random_seed = std::chrono::system_clock::now().time_since_epoch().count();
  } else {
    m_random_seed = random_seed;
  }

}

Hand::Hand(const Rules& rules, unsigned long random_seed)
{
  m_hand_ptr = std::make_shared<sheepshead::model::Hand> ();

  if(random_seed == 0) {
    m_random_seed = std::chrono::system_clock::now().time_since_epoch().count();
  } else {
    m_random_seed = random_seed;
  }

  auto new_rules = rules.m_hand_ptr->rule_variation();
  auto hand_rules = m_hand_ptr->mutable_rule_variation();
  *hand_rules = new_rules;
}

Hand::Hand(std::istream* input)
{
  m_hand_ptr = std::make_shared<sheepshead::model::Hand> ();
  m_hand_ptr->ParseFromIstream(input);
}

Hand::Hand(const std::string& input)
{
  m_hand_ptr = std::make_shared<sheepshead::model::Hand> ();
  m_hand_ptr->ParseFromString(input);
}

bool Hand::serialize(std::ostream* output) const
{
  return m_hand_ptr->SerializeToOstream(output);
}

bool Hand::serialize(std::string* output) const
{
  return m_hand_ptr->SerializeToString(output);
}

PlayerItr Hand::dealer() const
{
  return PlayerItr(m_hand_ptr, 0);
}

Rules Hand::rules() const
{
  return Rules(m_hand_ptr);
}

History Hand::history() const
{
  return History(m_hand_ptr);
}

Seat Hand::seat(PlayerId playerid) const
{
  return Seat(m_hand_ptr, playerid);
}

Playmaker Hand::playmaker(PlayerId playerid)
{
  return Playmaker(m_hand_ptr, playerid);
}

std::vector<Play> Hand::available_plays(PlayerId playerid) const
{
  std::vector<Play> plays;

  // If the hand cannot be played, just return an empty vector.
  if(!internal::is_playable(m_hand_ptr)) return plays;

  // The pick decision
  if(internal::ready_for_pick_play(m_hand_ptr) == playerid) {
    // Handle the last picker forced pick rule
    if(*std::prev(History(m_hand_ptr).picking_round().leader()) == playerid &&
       Rules(m_hand_ptr).no_picker_forced_pick()) {
      plays.emplace_back(Play::PlayType::PICK, PickDecision::PICK);
    } else {
      plays.emplace_back(Play::PlayType::PICK, PickDecision::PICK);
      plays.emplace_back(Play::PlayType::PICK, PickDecision::PASS);
  }
    return plays;
  }

  // The loner decision
  if(internal::ready_for_loner_play(m_hand_ptr) == playerid) {
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
  if(internal::ready_for_partner_play(m_hand_ptr) == playerid) {
    auto cards = internal::get_permitted_partner_cards(m_hand_ptr);
    for(auto card : cards) {
      plays.emplace_back(Play::PlayType::PARTNER, card);
    }
    return plays;
  }

  // The unknown card decision
  if(internal::ready_for_unknown_play(m_hand_ptr) == playerid) {

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
  if(internal::ready_for_discard_play(m_hand_ptr) == playerid) {
    auto discard_vectors = internal::get_permitted_discards(m_hand_ptr);
    for(auto& discard_vector : discard_vectors) {
      plays.emplace_back(Play::PlayType::DISCARD, discard_vector);
    }
    return plays;
  }

  // A trick card decision
  if(internal::ready_for_trick_play(m_hand_ptr) == playerid) {
    auto trick_cards = internal::get_permitted_trick_plays(m_hand_ptr);
    for(auto& trick_card : trick_cards) {
      plays.emplace_back(Play::PlayType::TRICK_CARD, trick_card);
    }
    return plays;
  }

  return plays;

}

Arbiter Hand::arbiter()
{
  return Arbiter(m_hand_ptr, m_random_seed);
}

bool Hand::is_playable() const
{
  return Arbiter(m_hand_ptr, m_random_seed).is_playable();
}

bool Hand::is_arbitrable() const
{
  return Arbiter(m_hand_ptr, m_random_seed).is_arbitrable();
}

bool Hand::is_finished() const
{
  return Arbiter(m_hand_ptr, m_random_seed).is_finished();
}

int Hand::reward(PlayerId player_id) const
{
  // If the hand isn't finished, then there are no points yet.
  if(!is_finished()) return 0;

  bool is_leasters = history().picking_round().picker()->is_null();

  if(!is_leasters) {
    int picking_team_points = 0;
    int picking_team_tricks = 0;
    int other_team_points = 0;
    int other_team_tricks = 0;
    PlayerId picker_id = *(history().picking_round().picker());
    PlayerId partner_id = history().partner();

    // Add up the points from tricks
    for(auto trick_itr=history().tricks_begin(); trick_itr!=history().tricks_end(); trick_itr++) {

      if(trick_itr->winner() == picker_id || trick_itr->winner() == partner_id) {
        picking_team_points += trick_itr->point_value(true);
        picking_team_tricks++;
      } else {
        other_team_points += trick_itr->point_value(true);
        other_team_tricks++;
      }
    }

    // And give the discards to the picker
    for(auto card : history().picking_round().discarded_cards()) {
      picking_team_points += card.point_value();
    }

    assert(picking_team_points + other_team_points == 120);
    // Now see how the picking team did, handling no-trickers as well
    int picking_team_win_magnitude = 0;
    if(picking_team_tricks == 0) {
      picking_team_win_magnitude = -3;
    } else if(31 > picking_team_points && picking_team_points >= 0) {
      picking_team_win_magnitude = -2;
    } else if(61 > picking_team_points && picking_team_points >= 31) {
      picking_team_win_magnitude = -1;
    } else if(91 > picking_team_points && picking_team_points >= 61) {
      picking_team_win_magnitude = 1;
    } else if(121 > picking_team_points && picking_team_points >= 91) {
      picking_team_win_magnitude = 2;
    }

    if(other_team_tricks == 0) {
      picking_team_win_magnitude = 3;
    }

    // And adjust based on team and whether this was a loner or not
    if(player_id == picker_id) {
      if(partner_id.is_null()) {
        return picking_team_win_magnitude * (rules().number_of_players() - 1);
      } else {
        return picking_team_win_magnitude * (rules().number_of_players() - 2) * 2 / 3;
      }
    } else if(player_id == partner_id) {
      return picking_team_win_magnitude * (rules().number_of_players() - 2) / 3;
    } else {
      return -1 * picking_team_win_magnitude;
    }

  } else { // Now handle the crazy world of leasters
    // You have to take at least one trick to win leasters
    std::map<PlayerId, int> possible_winners;
    for(auto trick_itr=history().tricks_begin(); trick_itr!=history().tricks_end(); trick_itr++) {
      possible_winners[trick_itr->winner()] += trick_itr->point_value(true);
    }

    auto leasters_winner = std::min_element(possible_winners.begin(), possible_winners.end(),
        [](std::pair<PlayerId, int> c, std::pair<PlayerId, int> d)
          {return c.second < d.second;});

    if(player_id == leasters_winner->first) {
      return rules().number_of_players() - 1;
    } else {
      return -1;
    }

  }

}

PlayerId Hand::current_player() const
{
  if(!is_playable()) {
    return PlayerId();
  }

  auto player_itr = dealer();
  do {
    auto available_plays_ = available_plays(*player_itr);
    if(available_plays_.size() > 0) {
      break;
    }
    ++player_itr;
  } while(true);

  return *player_itr;
}

Hand::TurnType Hand::current_turn() const
{
  auto player = current_player();
  if(player.is_null()) {
    return TurnType::NOT_PLAYABLE;
  }
  auto available_plays_ = available_plays(player);
  TurnType turn_type;
  switch(available_plays_[0].play_type()) {
    case Play::PlayType::PICK: turn_type = TurnType::PICK; break;
    case Play::PlayType::LONER: turn_type = TurnType::LONER; break;
    case Play::PlayType::PARTNER: turn_type = TurnType::PARTNER; break;
    case Play::PlayType::UNKNOWN: turn_type = TurnType::UNKNOWN; break;
    case Play::PlayType::DISCARD: turn_type = TurnType::DISCARD; break;
    case Play::PlayType::TRICK_CARD:
      switch(history().number_of_finished_tricks()) {
        case 0: turn_type = TurnType::TRICK_0; break;
        case 1: turn_type = TurnType::TRICK_1; break;
        case 2: turn_type = TurnType::TRICK_2; break;
        case 3: turn_type = TurnType::TRICK_3; break;
      }
  }
  return turn_type;
}

std::string Hand::debug_string() const
{
  std::stringstream out_stream;
  out_stream << "Hand dealt by " << dealer()->debug_string() << std::endl;
  if(is_playable()) out_stream << "Hand is playable." << std::endl;
  if(is_arbitrable()) out_stream << "Hand is arbitrable." << std::endl;
  if(is_finished()) out_stream << "Hand is finished." << std::endl;

  out_stream << "Hand history:" << std::endl;
  out_stream << history().debug_string() << std::endl;

  out_stream << "Current seats:" << std::endl;
  auto player_itr = dealer();
  do {
    out_stream << seat(*player_itr).debug_string() << std::endl;
    ++player_itr;
  } while(player_itr != dealer());

  return out_stream.str();
}

} // namespace interface
} // namespade sheepshead
