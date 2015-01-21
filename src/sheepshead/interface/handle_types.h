#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_HANDLETYPES_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_HANDLETYPES_H_

#include "sheepshead/proto/game.pb.h"
#include <memory>

namespace sheepshead {
namespace interface {

using ConstHandHandle = std::shared_ptr<const sheepshead::model::Hand>;
using MutableHandHandle = std::shared_ptr<sheepshead::model::Hand>;

}
}
#endif
