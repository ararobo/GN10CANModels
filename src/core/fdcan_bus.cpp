#include "gn10_can/core/fdcan_bus.hpp"

#include <cstddef>

#include "gn10_can/core/can_id.hpp"
#include "gn10_can/core/fdcan_device.hpp"

namespace gn10_can {

FDCANBus::FDCANBus(drivers::IFDCANDriver& driver) : driver_(driver), devices_{}, device_count_(0) {}

void FDCANBus::update()
{
    FDCANFrame frame;
    while (driver_.receive(frame)) {
        dispatch(frame);
    }
}

void FDCANBus::dispatch(const FDCANFrame& frame)
{
    uint32_t routing_id = frame.get_routing_id();

    for (std::size_t i = 0; i < device_count_; i++) {
        FDCANDevice* device = devices_[i];
        if (!device) {
            continue;
        }

        if (routing_id == device->get_routing_id()) {
            device->on_receive(frame);
        }
    }
}

bool FDCANBus::send_frame(const FDCANFrame& frame)
{
    return driver_.send(frame);
}

bool FDCANBus::attach(FDCANDevice* device)
{
    if (device_count_ < MAX_DEVICES && device != nullptr) {
        devices_[device_count_++] = device;
        return true;
    }
    return false;
}

void FDCANBus::detach(FDCANDevice* device)
{
    for (std::size_t i = 0; i < device_count_; i++) {
        if (devices_[i] == device) {
            // 見つかった場所を削除し、最後の要素を持ってきて穴埋めする（Orderは変わるが効率的）
            // ポインタを移動
            devices_[i]             = devices_[--device_count_];
            devices_[device_count_] = nullptr;  // Clear logic
            return;
        }
    }
}

}  // namespace gn10_can
