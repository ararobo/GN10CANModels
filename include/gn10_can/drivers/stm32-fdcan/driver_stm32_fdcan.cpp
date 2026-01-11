#include "driver_stm32_fdcan.hpp"

namespace gn10_can {
namespace drivers {

bool DriverSTM32FDCAN::init() {
    FDCAN_FilterTypeDef filter;
    filter.IdType       = FDCAN_STANDARD_ID;
    filter.FilterIndex  = 0;
    filter.FilterType   = FDCAN_FILTER_MASK;
    filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filter.FilterID1    = 0x000;
    filter.FilterID2    = 0x000;

    if (HAL_FDCAN_ConfigFilter(hfdcan_, &filter) != HAL_OK) {
        return false;
    }
    if (HAL_FDCAN_Start(hfdcan_) != HAL_OK) {
        return false;
    }
    if (HAL_FDCAN_ActivateNotification(hfdcan_, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        return false;
    }
    return true;
}

bool DriverSTM32FDCAN::send(const CANFrame& frame) {
    FDCAN_TxHeaderTypeDef tx_header;
    tx_header.IdType              = (frame.is_extended) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
    tx_header.Identifier          = frame.id;
    tx_header.TxFrameType         = FDCAN_DATA_FRAME;
    tx_header.DataLength          = frame.dlc;
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch       = FDCAN_BRS_OFF;
    tx_header.FDFormat            = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker       = 0;

    if (HAL_FDCAN_AddMessageToTxFifoQ(
            hfdcan_, &tx_header, const_cast<uint8_t*>(frame.data.data())) != HAL_OK) {
        return false;
    }
    return true;
}

bool DriverSTM32FDCAN::receive(CANFrame& out_frame) {
    FDCAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    if (HAL_FDCAN_GetRxMessage(hfdcan_, FDCAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK) {
        return false;
    }

    out_frame.id          = rx_header.Identifier;
    out_frame.dlc         = rx_header.DataLength;
    out_frame.is_extended = (rx_header.IdType == FDCAN_EXTENDED_ID);
    out_frame.is_rtr      = (rx_header.RxFrameType == FDCAN_REMOTE_FRAME);
    out_frame.is_error    = false;

    for (uint8_t i = 0; i < out_frame.dlc; ++i) {
        out_frame.data[i] = rx_data[i];
    }

    return true;
}

}  // namespace drivers
}  // namespace gn10_can