#pragma once
#include "gn10_can/core/fdcan_device.hpp"

namespace gn10_can {
namespace devices {

/**
 * @brief RobotControlHubのクライアント用デバイスクラス(PCなどを想定)
 *
 * @tparam Command 指令値のデータ構造体
 * @tparam Feedback フィードバックのデータ構造体
 */
template <typename Command, typename Feedback>
class RobotControlHubClient : public FDCANDevice
{
public:
    RobotControlHubClient(FDCANBus& bus, uint8_t dev_id)
        : FDCANDevice(bus, id::DeviceType::RobotControlHub, dev_id)
    {
        static_assert(sizeof(Command) <= 64, "Command size exceeds FDCAN limit (64bytes)");
        static_assert(sizeof(Feedback) <= 64, "Feedback size exceeds FDCAN limit (64bytes)");
    }

private:
};

}  // namespace devices
}  // namespace gn10_can