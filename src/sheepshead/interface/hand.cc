#include "hand.h"

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
