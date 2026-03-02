# 移植ガイド (Porting Guide)

このドキュメントでは、`gn10-can` を新しいマイコンやハードウェアに移植する手順を説明します。

---

## 目次

1. [ドライバの追加（新マイコン対応）](#1-ドライバの追加新マイコン対応)
2. [デバイスの追加（新周辺機器対応）](#2-デバイスの追加新周辺機器対応)

---

## 1. ドライバの追加（新マイコン対応）

### 1.1 実装すべきインターフェース

`drivers/driver_interface.hpp` の `ICanDriver` を継承し、2つの純粋仮想関数を実装します。

```cpp
namespace gn10_can {
namespace drivers {

class ICanDriver
{
public:
    virtual bool send(const CANFrame& frame) = 0;
    virtual bool receive(CANFrame& out_frame) = 0;
};

} // namespace drivers
} // namespace gn10_can
```

### 1.2 `receive()` の契約（最重要）

**`receive()` は必ず非ブロッキングで実装してください。**

| 状況 | 戻り値 | 動作 |
| :--- | :--- | :--- |
| 受信バッファにフレームがある | `true` | `out_frame` にフレームを書き込む |
| 受信バッファが空 | `false` | `out_frame` の内容は変更しない |

`bus.update()` は `receive()` が `false` を返すまで繰り返し呼び出します。
**ブロッキング実装をするとメインループが停止します。**

```cpp
// OK: ポーリング（非ブロッキング）
bool receive(CANFrame& out_frame) override
{
    if (HAL_CAN_GetRxFifoFillLevel(&hcan_, CAN_RX_FIFO0) == 0) {
        return false;  // データなし → 即座に返す
    }
    // フレームを読み取って out_frame に格納する処理
    ...
    return true;
}

// NG: ブロッキング（データが来るまで待機する実装は禁止）
bool receive(CANFrame& out_frame) override
{
    while (HAL_CAN_GetRxFifoFillLevel(&hcan_, CAN_RX_FIFO0) == 0);  // ← 禁止
    ...
}
```

### 1.3 実装例: ESP32 (Arduino)

`drivers/esp32_can/` に以下の2ファイルを作成します。

```cpp
// drivers/esp32_can/driver_esp32_can.hpp
#pragma once
#include <CAN.h>
#include <cstdint>
#include "gn10_can/drivers/driver_interface.hpp"

namespace gn10_can {
namespace drivers {

/**
 * @brief ESP32 Arduino 環境向け CAN ドライバ実装
 * @note Arduino の CAN ライブラリ (arduino-CAN) をラップします。
 */
class DriverESP32CAN : public ICanDriver
{
public:
    /**
     * @brief RX/TX ピンを設定する
     * @param rx RXピン番号
     * @param tx TXピン番号
     */
    void set_pin(uint8_t rx, uint8_t tx);

    /**
     * @brief CAN ペリフェラルを起動する
     * @param baud_rate ボーレート (例: 1000E3 = 1Mbps)
     * @return true 起動成功
     * @return false 起動失敗
     */
    bool begin(long baud_rate);

    bool send(const CANFrame& frame) override;
    bool receive(CANFrame& out_frame) override;
};

}  // namespace drivers
}  // namespace gn10_can
```

```cpp
// drivers/esp32_can/driver_esp32_can.cpp
#include "driver_esp32_can.hpp"

namespace gn10_can {
namespace drivers {

void DriverESP32CAN::set_pin(uint8_t rx, uint8_t tx)
{
    CAN.setPins(rx, tx);
}

bool DriverESP32CAN::begin(long baud_rate)
{
    return CAN.begin(baud_rate);
}

bool DriverESP32CAN::send(const CANFrame& frame)
{
    CAN.beginPacket(frame.id);
    CAN.write(frame.data.data(), frame.dlc);
    return CAN.endPacket();
}

bool DriverESP32CAN::receive(CANFrame& out_frame)
{
    // parsePacket() は受信データがなければ 0 を返す（非ブロッキング）
    int packet_size = CAN.parsePacket();
    if (packet_size == 0) {
        return false;
    }

    out_frame.id  = CAN.packetId();
    out_frame.dlc = static_cast<uint8_t>(packet_size);
    CAN.readBytes(out_frame.data.data(), packet_size);
    return true;
}

}  // namespace drivers
}  // namespace gn10_can
```

使用例（メカナムホイールロボット）:

> **⚠️ `set_init()` を呼ばないとモーターは動きません。**
> `MotorDriverServer` 側は `Init` フレームを受信するまで目標値指令を無視します。
> 必ず `setup()` 内で `set_init()` を送信してください。

```cpp
#include <Arduino.h>
#include <PS4Controller.h>
#include <gn10_can/core/can_bus.hpp>
#include <gn10_can/devices/motor_driver_client.hpp>
#include "drivers/esp32_can/driver_esp32_can.hpp"

/// PS4スティックのデッドゾーン比率 (10%)
constexpr float DEADZONE_RATIO = 0.1f;

gn10_can::drivers::DriverESP32CAN can_driver;
gn10_can::CANBus can_bus{can_driver};
gn10_can::devices::MotorDriverClient motor_fr{can_bus, 0};
gn10_can::devices::MotorDriverClient motor_fl{can_bus, 1};
gn10_can::devices::MotorDriverClient motor_rr{can_bus, 2};
gn10_can::devices::MotorDriverClient motor_rl{can_bus, 3};

/**
 * @brief PS4スティック入力にデッドゾーンを適用し、正規化された値を返す
 * @param raw    PS4スティックの生値 (-128 ~ 127)
 * @param deadzone_ratio デッドゾーンの比率 (0.0 ~ 1.0)
 * @return デッドゾーン適用済みの正規化値 (-1.0 ~ 1.0)
 */
static float apply_deadzone(int8_t raw, float deadzone_ratio)
{
    const float normalized = raw / 128.0f;
    if (fabsf(normalized) < deadzone_ratio) {
        return 0.0f;
    }
    return normalized;
}

void setup()
{
    Serial.begin(115200);

    can_driver.set_pin(/*rx=*/32, /*tx=*/33);
    if (!can_driver.begin(1000E3)) {
        Serial.println("failed to initialize CAN driver!");
        while (true) { delay(1000); }
    }

    // ⚠️ モーターが動くために必須: set_init() で設定を送信する
    gn10_can::devices::MotorConfig config;
    config.set_max_duty_ratio(1.0f);
    config.set_accel_ratio(1.0f);
    config.set_encoder_type(gn10_can::devices::EncoderType::None);
    config.set_feedback_cycle(100);
    motor_fr.set_init(config);
    motor_fl.set_init(config);
    motor_rr.set_init(config);
    motor_rl.set_init(config);

    PS4.begin("MACアドレス入れて");
    while (!PS4.isConnected()) { delay(1000); }
}

void loop()
{
    // 受信フレームを各デバイスに配送
    can_bus.update();

    if (!PS4.isConnected()) { return; }

    // デッドゾーン適用済みのスティック入力を取得
    float vx    = apply_deadzone(PS4.LStickX(), DEADZONE_RATIO) / 256.0f;
    float vy    = apply_deadzone(PS4.LStickY(), DEADZONE_RATIO) / 256.0f;
    float omega = apply_deadzone(PS4.RStickX(), DEADZONE_RATIO) / 256.0f;

    // メカナムホイール逆運動学で各車輪速度を計算して送信
    motor_fr.set_target( vx - vy + omega);
    motor_fl.set_target( vx + vy - omega);
    motor_rr.set_target( vx + vy + omega);
    motor_rl.set_target( vx - vy - omega);

    delay(10);  // 100Hz 制御ループ
}
```

### 1.4 ファイル配置

```
drivers/
├── esp32_can/              ← 追加例
│   ├── driver_esp32_can.hpp
│   └── driver_esp32_can.cpp
├── stm32_can/              ← 既存
│   ├── driver_stm32_can.hpp
│   └── driver_stm32_can.cpp
└── stm32_fdcan/            ← 既存
    ├── driver_stm32_fdcan.hpp
    └── driver_stm32_fdcan.cpp
```

### 1.5 参考: STM32 bxCAN (リポジトリ実装)

`drivers/stm32_can/` に既存の実装があります。HAL ベースの構成の参考にしてください。

```cpp
// drivers/stm32_can/driver_stm32_can.hpp (抜粋)
class DriverSTM32CAN : public ICanDriver
{
public:
    explicit DriverSTM32CAN(CAN_HandleTypeDef* hcan);

    bool init();   // フィルタ設定・CAN開始・割り込み有効化
    bool send(const CANFrame& frame) override;
    bool receive(CANFrame& out_frame) override;  // HAL_CAN_GetRxMessage を使用

private:
    CAN_HandleTypeDef* hcan_;
};
```

> `receive()` は `HAL_CAN_GetRxMessage()` が失敗した場合に即座に `false` を返します。
> 割り込み (`CAN_IT_RX_FIFO0_MSG_PENDING`) と組み合わせて使うことを想定しています。

---

## 2. デバイスの追加（新周辺機器対応）

サーボドライバや新センサーなど、新しいデバイスを追加する手順です。
[このプルリクエストを参考にしてください](https://github.com/ararobo/gn10-can/pull/44)

### 2.1 CAN ID の割り当て

`include/gn10_can/core/can_id.hpp` の `DeviceType` と対応する `MsgType*` 列挙型に追加します。

```cpp
// can_id.hpp に追加
enum class DeviceType : uint8_t {
    // ... 既存
    MyNewDevice = 7,  // 空き番号を使う
};

enum class MsgTypeMyNewDevice : uint8_t {
    Init     = 0,
    Command  = 1,
    Feedback = 2,
};
```

### 2.2 Client クラスの実装

`CANDevice` を継承し、「送信」と「受信解釈」を実装します。

```cpp
// include/gn10_can/devices/my_new_device_client.hpp
#pragma once
#include "gn10_can/core/can_device.hpp"

namespace gn10_can {
namespace devices {

/**
 * @brief MyNewDevice クライアント側制御クラス
 */
class MyNewDeviceClient : public CANDevice
{
public:
    MyNewDeviceClient(CANBus& bus, uint8_t dev_id);

    void set_target(float value);
    float feedback_value() const;

    void on_receive(const CANFrame& frame) override;

private:
    float feedback_value_{0.0f};
};

} // namespace devices
} // namespace gn10_can
```

`set_target()` と `on_receive()` の実装は `motor_driver_client.cpp` と同じパターンで記述します。

> **`send()` について:** `CANDevice` が `protected` メンバとして `send(command, payload)` を提供しています。
> 内部で `CANFrame` を組み立て、コンストラクタで受け取った `bus_` の `send_frame()` に渡します。
> 継承先は `send()` を呼ぶだけで送信でき、フレームの組み立て方を意識する必要はありません。

```cpp
// src/devices/my_new_device_client.cpp
#include "gn10_can/devices/my_new_device_client.hpp"
#include "gn10_can/utils/can_converter.hpp"

namespace gn10_can {
namespace devices {

/**
 * @brief コンストラクタ
 *
 * CANDevice のコンストラクタに DeviceType と dev_id を渡すことで、
 * このデバイスの CAN ID ルーティングが確定します。
 * また、コンストラクタ内で bus.attach(this) が自動的に呼ばれ、
 * バスへの登録が完了します（手動登録不要）。
 */
MyNewDeviceClient::MyNewDeviceClient(CANBus& bus, uint8_t dev_id)
    : CANDevice(bus, id::DeviceType::MyNewDevice, dev_id)
{
}

void MyNewDeviceClient::set_target(float value)
{
    // float (4 byte) をペイロードに pack して送信
    std::array<uint8_t, 4> payload{};
    converter::pack(payload, /*start_byte=*/0, value);
    send(id::MsgTypeMyNewDevice::Command, payload);
}

float MyNewDeviceClient::feedback_value() const
{
    return feedback_value_;
}

/**
 * @brief 受信フレームの処理
 *
 * CANBus は DeviceType + DeviceID が一致するフレームのみここに渡すため、
 * Command ビットだけを判定すればよい。
 */
void MyNewDeviceClient::on_receive(const CANFrame& frame)
{
    auto id_fields = id::unpack(frame.id);

    if (!id_fields.is_command(id::MsgTypeMyNewDevice::Feedback)) {
        return;
    }

    // unpack は範囲外アクセスを防ぎ、失敗時は false を返す
    float value;
    if (converter::unpack(frame.data, /*start_byte=*/0, value)) {
        feedback_value_ = value;
    }
}

} // namespace devices
} // namespace gn10_can
```

### 2.3 ファイル配置

```
include/gn10_can/devices/
├── my_new_device_client.hpp   ← 追加
└── my_new_device_server.hpp   ← 追加

src/devices/
├── my_new_device_client.cpp   ← 追加
└── my_new_device_server.cpp   ← 追加
```

### 2.4 テストの追加

新しいデバイスを追加したら、必ず `tests/test_my_new_device.cpp` を作成してください。
テストの書き方は [testing.md](testing.md) を参照してください。
