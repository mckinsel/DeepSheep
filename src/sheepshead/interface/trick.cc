#include "trick.h"

#include "sheepshead/interface/rules.h"

template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params)  
{
  return std::unique_ptr<T> (new T(std::forward<Ts>(params)...));
}

namespace sheepshead {
namespace interface {

// Trick

Trick::Trick()
  : m_hand_ptr(nullptr), m_trick_number(-1)
{}

Trick::Trick(const ConstHandHandle& hand_ptr, int trick_number)
  : m_hand_ptr(hand_ptr), m_trick_number(trick_number)
{}

bool Trick::is_started() const
{
  if(m_trick_number < 0)
    return false;

  if(m_hand_ptr->tricks_size() <= m_trick_number)
    return false;
  
  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  if(model_trick.laid_cards_size() == 0)
    return false;

  return true;
}

bool Trick::is_null()
{
  return m_hand_ptr == nullptr || m_trick_number < 0;
}

bool Trick::is_finished() const
{
  if(!this->is_started())
    return false;

  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  auto rules = Rules(m_hand_ptr);

  return model_trick.laid_cards_size() == rules.number_of_players();
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

//PlayerId Trick::card_played_by_player(PlayerId) const
//{}


// ConstTrickIterator
ConstTrickIterator::ConstTrickIterator(const ConstHandHandle& hand_ptr, int trick_number)
  : m_hand_ptr(hand_ptr), m_trick_number(trick_number)
{
  if(m_trick_number >= m_hand_ptr->tricks_size()) {
    m_trick_uptr = make_unique<const Trick> ();
  } else {
    m_trick_uptr = make_unique<const Trick> (hand_ptr, trick_number);
  }
}

bool ConstTrickIterator::operator==(const ConstTrickIterator& rhs) const
{
  return m_hand_ptr == rhs.m_hand_ptr && m_trick_number == rhs.m_trick_number;
}

bool ConstTrickIterator::operator!=(const ConstTrickIterator& rhs) const
{
  return !(*this == rhs);
}

const Trick& ConstTrickIterator::operator*()
{
  return *m_trick_uptr;
}

ConstTrickIterator& ConstTrickIterator::operator++()
{
  m_trick_number++;
  if(m_trick_number >= m_hand_ptr->tricks_size()) {
    m_trick_uptr = make_unique<const Trick> ();
  } else {
    m_trick_uptr = make_unique<const Trick> (m_hand_ptr, m_trick_number);
  }
  return *this; 
} 

} // namespace interface
} // namespace sheepshead
