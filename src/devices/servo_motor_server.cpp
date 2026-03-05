#include "gn10_can/devices/servo_motor_server.hpp"
#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

ServoMotorServer::ServoMotorServer(CANBus& bus, uint8_t device_id)
    : CANDevice(bus, id::DeviceType::ServoMotor, device_id)
{
}

/**
 * @brief 新しい初期化設定があれば更新する
 *
 * @param min_us 最小パルス幅 [us]
 * @param max_us 最大パルス幅 [us]
 * @return true 新しい設定があり更新した
 * @return false 新しい設定はなく、更新しなかった
 */
bool ServoMotorServer::get_new_init(uint16_t& min_us, uint16_t& max_us)
{
    if (init_.has_value()) {
        min_us = init_->min_us;
        max_us = init_->max_us;
        init_.reset();
        return true;
    }
    return false;
}

/**
 * @brief 新しい目標角度があれば更新する
 *
 * @param angle_rad 目標角度 [rad]
 * @return true 新しい目標角度があり更新した
 * @return false 新しい目標角度はなく、更新しなかった
 */
bool ServoMotorServer::get_new_angle_rad(float& angle_rad)
{
    if (angle_rad_.has_value()) {
        angle_rad = angle_rad_.value();
        angle_rad_.reset();
        return true;
    }
    return false;
}

/**
 * @brief CANパケット受信時の呼び出し関数の実装
 *
 * @param frame 受信したCANパケット
 */
void ServoMotorServer::on_receive(const CANFrame& frame)
{
    auto id_fields = id::unpack(frame.id);

    // Init: min_us (2bytes) + max_us (2bytes)
    if (id_fields.is_command(id::MsgTypeServoMotor::Init)) {
        uint16_t min_us = 0;
        uint16_t max_us = 0;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, min_us) &&
            converter::unpack(frame.data.data(), frame.dlc, 2, max_us)) {
            init_ = InitConfig{min_us, max_us};
        }
    }
    // AngleRad: angle_rad (4bytes)
    else if (id_fields.is_command(id::MsgTypeServoMotor::AngleRad)) {
        float val = 0.0f;
        if (converter::unpack(frame.data.data(), frame.dlc, 0, val)) {
            angle_rad_ = val;
        }
    }
}

}  // namespace devices
}  // namespace gn10_can