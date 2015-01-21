#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYERID_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_PLAYERID_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"

#include <memory>

namespace sheepshead {
namespace interface {

class PlayerId
{
public:
  PlayerId();
  PlayerId(const ConstHandHandle&, int);

  bool is_null();

  bool operator==(const PlayerId& other);
  bool operator!=(const PlayerId& other);

  PlayerId next();
  PlayerId previous();

private:
  const ConstHandHandle m_hand_ptr;
  const int m_position;

}; // class PlayerId

} // namespace interface
} // namespade sheepshead
#endif
