#pragma once

#include <vector>

#include "gn10_can/core/can_device.hpp"
#include "gn10_can/drivers/driver_interface.hpp"

namespace gn10_can {

class CANManager {
 public:
    explicit CANManager(drivers::DriverInterface& driver);

    void resister_device(CANDevice* device);

    void update();

    bool send_frame(const CANFrame& frame);

 private:
    drivers::DriverInterface& driver_;
    std::vector<CANDevice*> devices_;
};
}  // namespace gn10_can