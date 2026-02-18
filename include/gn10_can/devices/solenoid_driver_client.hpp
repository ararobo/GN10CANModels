#pragma once

#include "gn10_can/core/can_device.hpp"

namespace gn10_can {
namespace devices {

class SolenoidDriverClient : CANDevice {
  public:
    /**
     * @brief ソレノイド用デバイスクラスのコンストラクタ
     *
     * @param bus CANBusクラスの参照
     * @param dev_id デバイスID
     */
    SolenoidDriverClient(CANBus& bus, uint8_t dev_id);

  private:
    /* data */
};

}  // namespace devices
}  // namespace gn10_can