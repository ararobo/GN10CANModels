#pragma once
#include <optional>

#include "gn10_can/core/fdcan_device.hpp"

namespace gn10_can {
namespace devices {

/**
 * @brief RobotControlHubのサーバー用デバイスクラス(ロボットを統括し、PCとの橋渡しを担う基板)
 *
 * @tparam Command 指令値のデータ構造体
 * @tparam Feedback フィードバックのデータ構造体
 */
template <typename Command, typename Feedback>
class RobotControlHubServer : public FDCANDevice
{
public:
    RobotControlHubServer(FDCANBus& bus, uint8_t dev_id)
        : FDCANDevice(bus, id::DeviceType::RobotControlHub, dev_id)
    {
        static_assert(sizeof(Command) <= 64, "Command size exceeds FDCAN limit (64bytes)");
        static_assert(sizeof(Feedback) <= 64, "Feedback size exceeds FDCAN limit (64bytes)");
    }

    bool get_command(Command& command)
    {
        if (command_.has_value()) {
            command = command_.value();
            command_.reset();
            return true;
        }
        return false;
    }

private:
    std::optional<Command> command_;
};

}  // namespace devices
}  // namespace gn10_can