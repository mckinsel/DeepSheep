#include "sheepshead/interface/history.h"
#include <algorithm>
#include <sstream>
#include <type_traits>

namespace sheepshead {
namespace interface {


History::History(const ConstHandHandle& hand_ptr)
  : m_hand_ptr(hand_ptr)
{}

PickingRound<decltype(History::m_hand_ptr)> History::picking_round() const
{
  return PickingRound<decltype(m_hand_ptr)>(m_hand_ptr);
}


TrickItr<decltype(History::m_hand_ptr)> History::tricks_begin() const
{

  return TrickItr<decltype(m_hand_ptr)>(m_hand_ptr, 0);
}

TrickItr<decltype(History::m_hand_ptr)> History::tricks_end() const
{
  return TrickItr<decltype(m_hand_ptr)>(m_hand_ptr, m_hand_ptr->tricks_size());
}

int History::number_of_started_tricks() const
{
  return m_hand_ptr->tricks_size();
}

int History::number_of_finished_tricks() const
{
  return std::count_if(this->tricks_begin(), this->tricks_end(),
                       [](const Trick<decltype(m_hand_ptr)>& t)
                         {return t.is_finished();});
}

PlayerId History::partner() const
{
  auto partner_card = picking_round().partner_card();
  if(partner_card.is_null()) {
    return PlayerId();
  }

  for(auto trick_itr=tricks_begin(); trick_itr!=tricks_end(); trick_itr++) {

    auto player_itr = trick_itr->leader();

    for(auto card_itr=trick_itr->laid_cards_begin(); card_itr!=trick_itr->laid_cards_end(); ++card_itr) {
      if(*card_itr == partner_card) {
        return *player_itr;
      }
      ++player_itr;
    }
  }

  // If the partner card has not yet been played, return the null player
  return PlayerId();
}

std::string History::debug_string() const
{
  std::stringstream out_stream;
  out_stream << picking_round().debug_string() << std::endl;
  for(auto trick=tricks_begin(); trick!=tricks_end(); trick++) {
    out_stream << trick->debug_string();
    out_stream << std::endl;
  }
  return out_stream.str();
}

Trick<decltype(History::m_hand_ptr)> History::latest_trick() const
{
  int num_started = this->number_of_started_tricks();

  if(num_started == 0) return Trick<decltype(m_hand_ptr)>();

  return Trick<decltype(m_hand_ptr)>(m_hand_ptr, num_started - 1);
}


} // namespace interface
} // namespace sheepshead
