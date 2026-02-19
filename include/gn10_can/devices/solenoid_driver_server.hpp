#pragma once

#include "gn10_can/core/can_device.hpp"

namespace gn10_can {
namespace devices {

class SolenoidDriverServer : CANDevice {
  public:
    /**
     * @brief ソレノイド用サーバークラスのコンストラクタ
     *
     * @param bus CANBusクラスの参照
     * @param dev_id デバイスID
     */
    SolenoidDriverServer(CANBus& bus, uint8_t dev_id);

    void target(bool& target);

  private:
    /* data */
};

}  // namespace devices
}  // namespace gn10_can