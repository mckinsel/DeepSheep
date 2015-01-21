#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_CHRONICLE_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_CHRONICLE_H_

#include "sheepshead/proto/game.pb.h"

#include "sheepshead/interface/handle_types.h"

namespace sheepshead {
namespace interface {

class Chronicle
{
public:
  Chronicle(const ConstHandHandle& hand_ptr); 

private:
  const ConstHandHandle m_hand_ptr;

}; // class Chronicle

} // namespace interface
} // namespace sheepshead
#endif
