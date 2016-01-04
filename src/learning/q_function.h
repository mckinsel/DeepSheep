#ifndef DEEPSHEEP_LEARNERS_QFUNCTION_H_
#define DEEPSHEEP_LEARNERS_QFUNCTION_H_

#include "sheepshead/interface/hand.h"
#include "sheepshead/interface/playmaker.h"

#include <vector>

namespace learning {

class Experience
{
public:
  Experience(const sheepshead::interface::Hand& start_state,
             const sheepshead::interface::Hand& end_state);

  sheepshead::interface::Play action() const;
  const sheepshead::interface::Hand& start_state() const;
  const sheepshead::interface::Hand& end_state() const;

private:
  const sheepshead::interface::Hand& m_start_state;
  const sheepshead::interface::Hand& m_end_state;
};

class QFunction
{
public:
  QFunction();

  float evaluate(const sheepshead::interface::Hand& state,
                 const sheepshead::interface::Play& action) const;
  
  void learn(const Experience& experience);
  std::string debug_string() const;
  int project(const sheepshead::interface::Hand& state, const sheepshead::interface::Play& play) const;

private:
  std::vector<float> m_q_values;
};

} // namespace learning
#endif
