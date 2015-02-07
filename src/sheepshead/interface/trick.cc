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
bool Trick<Handle_T>::is_null() const
{
  return m_hand_ptr == nullptr || m_trick_number < 0;
}

template<typename Handle_T>
bool Trick<Handle_T>::is_started() const
{
  if(this->is_null()) return false;

  if(m_hand_ptr->tricks_size() <= m_trick_number)
    return false;
  
  return m_hand_ptr->tricks_size() > m_trick_number;
}


template<typename Handle_T>
bool Trick<Handle_T>::is_finished() const
{
  if(!this->is_started()) // Checks is_null
    return false;

  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  auto rules = Rules(m_hand_ptr);

  return model_trick.laid_cards_size() == rules.number_of_players();
}

template<typename Handle_T>
PlayerItr Trick<Handle_T>::leader() const
{
  if(!this->is_started()) return PlayerItr(); // Checks is_null

  auto model_trick = m_hand_ptr->tricks(m_trick_number);
  return PlayerItr(m_hand_ptr, model_trick.leader_position());
}

template<typename Handle_T>
CardItr Trick<Handle_T>::laid_cards_begin() const
{
  if(!this->is_started()) return CardItr();

  auto&& laid_cards = m_hand_ptr->tricks(m_trick_number)
                      .laid_cards();
  return CardItr(m_hand_ptr, laid_cards.begin());
}

template<typename Handle_T>
CardItr Trick<Handle_T>::laid_cards_end() const
{
  if(!this->is_started()) return CardItr();

  auto&& laid_cards = m_hand_ptr->tricks(m_trick_number)
                      .laid_cards();
  return CardItr(m_hand_ptr, laid_cards.end());
}

template<typename Handle_T>
int Trick<Handle_T>::number_of_laid_cards() const
{
  if(!this->is_started()) return 0;

  return m_hand_ptr->tricks(m_trick_number).laid_cards_size();
}

/*PlayerId Trick::winner() const
{
  if(!(this->is_finished())) return PlayerId();
  
  // TODO: trick adjudication function
  return PlayerId();
}*/


template class Trick<ConstHandHandle>;
template class Trick<MutableHandHandle>;

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
bool TrickItr<Handle_T>::is_null() const
{
  return m_hand_ptr == nullptr;
}

template<typename Handle_T>
Trick<Handle_T>& TrickItr<Handle_T>::operator*()
{
  if(this->is_null()    ||
     m_trick_number < 0 ||
     m_trick_number >= m_hand_ptr->tricks_size())
  {
    m_trick = Trick<Handle_T>();
  } else {
    m_trick = Trick<Handle_T>(m_hand_ptr, m_trick_number);
  }

  return m_trick;
}

template<typename Handle_T>
Trick<Handle_T>* TrickItr<Handle_T>::operator->()
{
  return &(operator*());
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
