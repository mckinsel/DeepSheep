#include "sheepshead/interface/history.h"

namespace sheepshead {
namespace interface {

History::History(const ConstHandHandle& hand_ptr)
  : m_hand_ptr(hand_ptr)
{}

ConstTrickIterator History::tricks_cbegin() const
{
  return ConstTrickIterator(m_hand_ptr, 0);
}

ConstTrickIterator History::tricks_cend() const
{
  return ConstTrickIterator(m_hand_ptr, m_hand_ptr->tricks_size());
}

int History::number_of_tricks() const
{
  return m_hand_ptr->tricks_size();
}

} // namespace interface
} // namespace sheepshead
