#include "gn10_can/core/can_device.hpp"
#include "gn10_can/core/can_manager.hpp"

namespace gn10_can {

bool CANDevice::send_frame_internal(const CANFrame& frame) {
    if (manager_) {
        return manager_->send_frame(frame);
    }
    return false;
}

} // namespace gn10_can
