/**
 * @file robot_control_hub_server.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief ロボットを統括し、PCとの橋渡しを担うデバイスのサーバー(マイコン側)クラス
 * @version 0.1
 * @date 2026-04-12
 *
 * @copyright Copyright (c) 2026 ararobo
 *
 */
#pragma once
#include <optional>

#include "gn10_can/core/fdcan_device.hpp"
#include "gn10_can/utils/can_converter.hpp"

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

    void send_feedback(const Feedback& feedback)
    {
        FDCANFrame frame = FDCANFrame::make(
            id::DeviceType::RobotControlHub, device_id_, id::MsgTypeRobotControlHub::Feedback
        );
        converter::pack(frame.data, 0, feedback);
        frame.dlc = sizeof(Feedback);
        bus_.send_frame(frame);
    }

    void on_receive(const FDCANFrame& frame) override
    {
        auto id_fields = id::unpack(frame.id);
        if (id_fields.is_command(id::MsgTypeRobotControlHub::Command)) {
            if (frame.dlc == sizeof(Command)) {
                Command command;
                if (converter::unpack(frame.data.data(), frame.dlc, 0, command)) {
                    command_ = command;
                }
            }
        }
    }

private:
    std::optional<Command> command_;
};

}  // namespace devices
}  // namespace gn10_can