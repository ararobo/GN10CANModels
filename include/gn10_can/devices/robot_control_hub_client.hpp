#pragma once
#include "gn10_can/core/fdcan_device.hpp"
#include "gn10_can/utils/can_converter.hpp"

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

    void send_command(const Command& command)
    {
        FDCANFrame frame = FDCANFrame::make(
            id::DeviceType::RobotControlHub, device_id_, id::MsgTypeRobotControlHub::Command
        );
        converter::pack(frame.data, 0, command);
        frame.dlc = sizeof(Command);
        bus_.send_frame(frame);
    }

private:
};

}  // namespace devices
}  // namespace gn10_can