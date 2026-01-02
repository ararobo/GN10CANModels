#pragma once

#include <cstdint>
#include <type_traits>

namespace gn10_can {
namespace id {

static constexpr uint8_t BIT_WIDTH_DEV_TYPE = 3;
static constexpr uint8_t BIT_WIDTH_DEV_ID   = 4;
static constexpr uint8_t BIT_WIDTH_COMMAND  = 4;

enum class DeviceType : uint8_t {
    EmergencyStop       = 0,
    MotorDriver         = 1,
    ServoDriver         = 2,
    SolenoidDriver      = 3,
    CommunicationModule = 4,
    SensorHub           = 5,
    LED                 = 6,
    Reserved7           = 7
};

enum class MsgTypeEmergencyStop : uint8_t { Init = 0, Status = 1, EmergencyStop = 2 };

enum class MsgTypeMotorDriver : uint8_t {
    Init        = 0,
    Target      = 1,
    LimitSwitch = 2,
    Gain        = 3,
    Feedback    = 4,
};

enum class MsgTypeServoDriver : uint8_t {
    Init      = 0,
    Target    = 1,
    Frequency = 2,
};

enum class MsgTypeSolenoidDriver : uint8_t {
    Init   = 0,
    Target = 1,
};

enum class MsgTypeCommunicationModule : uint8_t {
    Init           = 0,
    Heartbeat      = 1,
    ControllerData = 2,
};

enum class MsgTypeSensorHub : uint8_t {
    Init = 0,
    ToF  = 1,
};

enum class MsgTypeLED : uint8_t {
    Init = 0,
};

}  // namespace id
}  // namespace gn10_can