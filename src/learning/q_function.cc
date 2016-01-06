#include "q_function.h"

#include <algorithm>

namespace learning {

QFunction::QFunction()
  : m_q_values(14, 0)
{}

int QFunction::project(const sheepshead::interface::Hand& state,
                       const sheepshead::interface::Play& play) const
{
  auto current_player = state.current_player();
  auto seat = state.seat(current_player);
  int trump_count = std::count_if(seat.held_cards_begin(), seat.held_cards_end(),
      [](sheepshead::interface::Card& c){return c.is_trump();});
  
  int state_action_id = trump_count;
  if(*(play.pick_decision()) == sheepshead::interface::PickDecision::PICK) {
    state_action_id += 7;
  }
  
  return state_action_id;
}

float QFunction::evaluate(const sheepshead::interface::Hand& state,
                          const sheepshead::interface::Play& play) const
{
  int state_action_id = project(state, play);
  return m_q_values[state_action_id];
}

std::string QFunction::debug_string() const
{
  std::string out_string = "QFunction internal values:\n";
  out_string += "PASS values:\n";
  for(int i=0; i<7;  i++) {
    out_string += std::to_string(m_q_values[i]);
    out_string += " ";
  }
  out_string += "\nPICK values:\n";
  for(int i=0; i<7;  i++) {
    out_string += std::to_string(m_q_values[i+7]);
    out_string += " ";
  }

  return out_string;
}

void QFunction::learn(const Experience& experience)
{
  int state_action_id = project(experience.start_state(), experience.action());
  auto current_player = experience.start_state().current_player();
  int reward = experience.end_state().reward(current_player);
  m_q_values[state_action_id] += .05*(reward - m_q_values[state_action_id]);
}

Experience::Experience(const sheepshead::interface::Hand& start_state,
                       const sheepshead::interface::Hand& end_state)
  : m_start_state(start_state), m_end_state(end_state)
{}

const sheepshead::interface::Hand& Experience::start_state() const
{
  return m_start_state;
}

const sheepshead::interface::Hand& Experience::end_state() const
{
  return m_end_state;
}

sheepshead::interface::Play Experience::action() const
{
  auto player_id = start_state().current_player();
  auto pick_decision = end_state().history().picking_round().pick_decision_by(player_id);
  return sheepshead::interface::Play(sheepshead::interface::Play::PlayType::PICK, pick_decision);
}

} // namespace learning
