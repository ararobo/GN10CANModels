#include "gn10_can/core/can_device.hpp"

namespace gn10_can {

CANDevice::CANDevice(CANManager& manager, id::DeviceType device_type, uint8_t device_id)
    : manager_(manager), device_type_(device_type), device_id_(device_id) {}
aaa
}  // namespace gn10_can