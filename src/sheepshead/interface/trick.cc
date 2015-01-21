#include "trick.h"

#include "sheepshead/interface/rules.h"

namespace sheepshead {
namespace interface {

Trick::Trick(const ConstHandHandle& hand_ptr, int trick_number)
  : m_hand_ptr(hand_ptr), m_trick_number(trick_number)
{
}

bool Trick::is_started() const
{
  if(m_hand_ptr->tricks_size() <= m_trick_number)
    return false;
  
  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  if(model_trick.laid_cards_size() == 0)
    return false;

  return true;
}

bool Trick::is_finished() const
{
  if(!this->is_started())
    return false;

  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  auto rules = Rules(m_hand_ptr);

  return model_trick.laid_cards_size() == rules.number_of_cards_per_trick();
}

PlayerId Trick::leader() const
{
  if(m_hand_ptr->tricks_size() <= m_trick_number)
    return PlayerId();

  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  return PlayerId(m_hand_ptr, model_trick.leader_position());
}

PlayerId Trick::winner() const
{
  if(!(this->is_finished())) return PlayerId();
  
  // TODO: trick adjudication function
  return PlayerId();
}

} // namespace interface
} // namespace sheepshead
