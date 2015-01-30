#ifndef DEEPSHEEP_SHEEPSHEAD_INTERFACE_HANDLETYPES_H_
#define DEEPSHEEP_SHEEPSHEAD_INTERFACE_HANDLETYPES_H_

#include "sheepshead/proto/game.pb.h"
#include <memory>

#define RESTRICT_TO_MODEL_HAND(T)                                                                  \
  template<typename U = T,                                                                         \
  typename std::enable_if<std::is_same<typename std::remove_cv<typename U::element_type>::type, \
                          sheepshead::model::Hand>::value>::type...>
namespace sheepshead {
namespace interface {

using ConstHandHandle = std::shared_ptr<const sheepshead::model::Hand>;
using MutableHandHandle = std::shared_ptr<sheepshead::model::Hand>;

}
}
#endif
