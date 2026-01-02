#pragma once

#include <string>

#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {
namespace drivers {

class DriverInterface {
 public:
    virtual ~DriverInterface() = default;

    virtual bool send(const CANFrame& frame) = 0;

    virtual bool receive(CANFrame& out_frame) = 0;
};
}  // namespace drivers
}  // namespace gn10_can