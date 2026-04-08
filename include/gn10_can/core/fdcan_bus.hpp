/**
 * @file can_bus.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief
 * 物理的なFDCANバスをソフトウェア上で表現したクラス。デバイスの接続(Attach)と、メッセージのルーティング(Dispatch)を担当します。
 * @version 0.1.0
 * @date 2026-04-01
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <array>
#include <cstddef>

#include "gn10_can/drivers/fdcan_driver_interface.hpp"

namespace gn10_can {

class FDCANDevice;

/**
 * @brief
 * 物理的なFDCANバスをソフトウェア上で表現したクラス。デバイスの接続(Attach)と、メッセージのルーティング(Dispatch)を担当します。
 *
 */
class FDCANBus
{
public:
    static constexpr std::size_t MAX_DEVICES = 16;  // 最大登録デバイス数

    /**
     * @brief FDCANBusクラスのコンストラクタ
     *
     * @param driver FDCANドライバーインターフェースの参照
     */
    explicit FDCANBus(drivers::IFDCANDriver& driver);

    /**
     * @brief FDCANパケットの受信とデバイスへのルーティング処理
     *
     * 受信データを読み込み、適切なデバイスに渡します。
     */
    void update();

    /**
     * @brief FDCANフレーム送信関数
     *
     * @param frame 送信するCANフレーム
     * @return true 送信成功
     * @return false 送信失敗
     */
    bool send_frame(const FDCANFrame& frame);

private:
    friend class FDCANDevice;

    /**
     * @brief デバイスをバスに接続する (RAII内部利用)
     *
     * FDCANDeviceのコンストラクタから自動的に呼び出されます。
     *
     * @param device 登録するデバイスへのポインタ
     * @return true 登録成功
     * @return false 登録失敗（デバイス数上限）
     */
    bool attach(FDCANDevice* device);

    /**
     * @brief デバイスをバスから切断する (RAII内部利用)
     *
     * FDCANDeviceのデストラクタから自動的に呼び出されます。
     *
     * @param device 登録解除するデバイスへのポインタ
     */
    void detach(FDCANDevice* device);

    /**
     * @brief 受信したフレームを適切なデバイスに配送する
     *
     * @param frame 受信フレーム
     */
    void dispatch(const FDCANFrame& frame);

    drivers::IFDCANDriver& driver_;                    // CANドライバーインターフェースの参照を保持
    std::array<FDCANDevice*, MAX_DEVICES> devices_{};  // 登録されているデバイスの配列
    std::size_t device_count_ = 0;                     // 登録されているデバイス数
};
}  // namespace gn10_can
