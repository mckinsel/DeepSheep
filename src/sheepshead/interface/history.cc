#include "sheepshead/interface/history.h"
#include <type_traits>

namespace sheepshead {
namespace interface {

template <typename T> class TD;

History::History(const ConstHandHandle& hand_ptr)
  : m_hand_ptr(hand_ptr)
{}

PickingRound<decltype(History::m_hand_ptr)> History::picking_round() const
{
  return PickingRound<decltype(m_hand_ptr)>(m_hand_ptr);
}


TrickItr<decltype(History::m_hand_ptr)> History::tricks_cbegin() const
{

  return TrickItr<decltype(m_hand_ptr)>(m_hand_ptr, 0);
}

TrickItr<decltype(History::m_hand_ptr)> History::tricks_cend() const
{
  return TrickItr<decltype(m_hand_ptr)>(m_hand_ptr, m_hand_ptr->tricks_size());
}

int History::number_of_started_tricks() const
{
  return m_hand_ptr->tricks_size();
}

} // namespace interface
} // namespace sheepshead
