#include "trick.h"

#include "sheepshead/interface/rules.h"

namespace sheepshead {
namespace interface {

// Trick

template<typename Handle_T>
Trick<Handle_T>::Trick()
  : m_hand_ptr(nullptr), m_trick_number(-1)
{}


template<typename Handle_T>
bool Trick<Handle_T>::is_started() const
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

template<typename Handle_T>
bool Trick<Handle_T>::is_null() const
{
  return m_hand_ptr == nullptr || m_trick_number < 0;
}

template<typename Handle_T>
bool Trick<Handle_T>::is_finished() const
{
  if(!this->is_started())
    return false;

  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  auto rules = Rules(m_hand_ptr);

  return model_trick.laid_cards_size() == rules.number_of_players();
}

template class Trick<ConstHandHandle>;
template class Trick<MutableHandHandle>;

/*PlayerId Trick::leader() const
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
}*/


//TrickItr
template<typename Handle_T>
TrickItr<Handle_T>::TrickItr()
  : m_hand_ptr(nullptr), m_trick_number(-1)
{}

template<typename Handle_T>
TrickItr<Handle_T>::TrickItr(const Handle_T& hand_ptr, int trick_number)
  : m_hand_ptr(hand_ptr), m_trick_number(trick_number)
{}

template<typename Handle_T>
TrickItr<Handle_T>& TrickItr<Handle_T>::operator++()
{
  m_trick_number++;
  return *this; 
} 

template<typename Handle_T>
TrickItr<Handle_T> TrickItr<Handle_T>::operator++(int)
{
  auto copy(*this);
  ++(*this); 
  return copy;
} 

template<typename Handle_T>
TrickItr<Handle_T>& TrickItr<Handle_T>::operator--()
{
  m_trick_number--;
  return *this; 
} 

template<typename Handle_T>
TrickItr<Handle_T> TrickItr<Handle_T>::operator--(int)
{
  auto copy(*this);
  --(*this); 
  return copy;
} 

template<typename Handle_T>
Trick<Handle_T>& TrickItr<Handle_T>::operator*()
{
  if(m_trick_number < 0 ||
     m_trick_number >= m_hand_ptr->tricks_size())
  {
    m_trick = Trick<Handle_T>();
  } else {
    m_trick = Trick<Handle_T>(m_hand_ptr, m_trick_number);
  }

  return m_trick;
}

template<typename Handle_T>
bool TrickItr<Handle_T>::operator==(const TrickItr<Handle_T>& rhs) const
{
  return m_hand_ptr == rhs.m_hand_ptr && m_trick_number == rhs.m_trick_number;
}

template<typename Handle_T>
bool TrickItr<Handle_T>::operator!=(const TrickItr<Handle_T>& rhs) const
{
  return !(*this == rhs);
}

template class TrickItr<ConstHandHandle>;
template class TrickItr<MutableHandHandle>;

} // namespace interface
} // namespace sheepshead
