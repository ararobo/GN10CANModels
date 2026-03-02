# テストガイド

このドキュメントでは、テストの実行方法と `MockDriver` を使ったテストの書き方を説明します。

---

## 目次

1. [テストの実行](#1-テストの実行)
2. [MockDriver の仕組み](#2-mockdriver-の仕組み)
3. [テストの書き方](#3-テストの書き方)
4. [新デバイス追加時のテスト方針](#4-新デバイス追加時のテスト方針)

---

## 1. テストの実行

### ビルドとテスト

```bash
# ビルド (初回 or CMakeLists.txt を変更したとき)
cmake --preset linux-ninja
cmake --build build/linux_ninja

# 全テストを実行
cd build/linux_ninja && ctest --output-on-failure

# 特定のテストだけ実行
ctest --output-on-failure -R test_motor_driver
```

### テストファイルの場所

```
tests/
├── test_can_bus.cpp        # CANBus の送受信・ルーティング
├── test_can_converter.cpp  # pack/unpack 変換
├── test_can_frame.cpp      # CANFrame 構造体
├── test_motor_driver.cpp   # MotorDriverClient / Server の通信
└── mock_driver.hpp         # テスト用ドライバ
```

---

## 2. MockDriver の仕組み

`MockDriver` は `ICanDriver` を実装したテスト専用クラスです。
実際のハードウェアなしに、フレームの送受信をメモリ上でシミュレートします。

```cpp
class MockDriver : public gn10_can::drivers::ICanDriver
{
public:
    // send() が呼ばれたフレームを蓄積する
    std::vector<gn10_can::CANFrame> sent_frames;

    // receive() のために手動でフレームを注入するキュー
    std::queue<gn10_can::CANFrame> receive_queue;

    bool send(const gn10_can::CANFrame& frame) override;   // sent_frames に追加
    bool receive(gn10_can::CANFrame& out_frame) override;  // receive_queue から取り出す

    // テスト内でフレームを注入するヘルパー
    void push_receive_frame(const gn10_can::CANFrame& frame);
};
```

### フレームの流れ

```
テストコード
    │
    ├─ device.send_something()
    │       └─> driver.sent_frames に蓄積
    │
    ├─ driver.push_receive_frame(...)  ← テストが手動で受信フレームを注入
    │
    └─ bus.update()
            └─> driver.receive() → dispatch() → device.on_receive()
```

---

## 3. テストの書き方

### 3.1 基本パターン (Client → Server の通信テスト)

`MotorDriverTest` のフィクスチャを参考にしてください。

```cpp
#include <gtest/gtest.h>
#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/devices/motor_driver_client.hpp"
#include "gn10_can/devices/motor_driver_server.hpp"
#include "mock_driver.hpp"

using namespace gn10_can;
using namespace gn10_can::devices;

class MotorDriverTest : public ::testing::Test
{
protected:
    MockDriver driver;
    CANBus bus{driver};
    MotorDriverClient client{bus, /*dev_id=*/1};
    MotorDriverServer server{bus, /*dev_id=*/1};

    void SetUp() override
    {
        driver.sent_frames.clear();
    }

    // Client が送信したフレームをバスに折り返す
    void ProcessBus()
    {
        for (const auto& frame : driver.sent_frames) {
            driver.push_receive_frame(frame);
        }
        driver.sent_frames.clear();
        bus.update();
    }
};

TEST_F(MotorDriverTest, Target)
{
    // Client → Server へ目標値を送信
    client.set_target(0.8f);
    ProcessBus();

    // Server が正しく受け取れているか検証
    float target = 0.0f;
    EXPECT_TRUE(server.get_new_target(target));
    EXPECT_NEAR(target, 0.8f, 1e-3f);
}
```

### 3.2 Server → Client のフィードバックテスト

```cpp
TEST_F(MotorDriverTest, Feedback)
{
    // Server → Client へフィードバックを送信
    server.send_feedback(/*feedback_val=*/1.5f, /*limit_switch_state=*/0);
    ProcessBus();

    // Client がフィードバックを受け取れているか検証
    EXPECT_NEAR(client.feedback_value(), 1.5f, 1e-3f);
}
```

### 3.3 フレームを直接注入するパターン

CANFrame を手動で組み立てて注入する場合に使います。

```cpp
TEST_F(MotorDriverTest, DirectFrameInjection)
{
    // CANFrame を直接組み立てて注入する
    float value = 2.0f;
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);

    CANFrame frame = CANFrame::make(
        gn10_can::id::DeviceType::MotorDriver,
        /*dev_id=*/1,
        gn10_can::id::MsgTypeMotorDriver::Feedback,
        bytes, 4
    );

    driver.push_receive_frame(frame);
    bus.update();

    EXPECT_NEAR(client.feedback_value(), 2.0f, 1e-3f);
}
```

### 3.4 値が届いていない場合のテスト

`get_new_*()` は値が未到達のとき `false` を返すことを検証します。

```cpp
TEST_F(MotorDriverTest, NoNewTarget)
{
    // 何も送信せずに読み出す → false が返るべき
    float target = 0.0f;
    EXPECT_FALSE(server.get_new_target(target));
}
```

---

## 4. 新デバイス追加時のテスト方針

新しいデバイスクラスを追加したら、以下の項目をテストしてください。

| テスト項目 | 説明 |
| :--- | :--- |
| Command ごとの送受信 | Client の各 `send_*()` が Server の `get_new_*()` で正しく受け取れること |
| フィードバック | Server の `send_*()` が Client の getter で正しく取得できること |
| 無効フレームの無視 | 異なる dev_id や無関係な Command のフレームを無視すること |
| 初期値 | インスタンス生成直後の getter が適切なデフォルト値（0など）を返すこと |
| 未受信時の `get_new_*()` | フレームが届いていない状態では `false` を返すこと |

### テストファイルの命名規則

```
tests/test_<デバイス名>.cpp
```

例: `SolenoidDriver` → `tests/test_solenoid_driver.cpp`

### CMakeLists.txt への登録

`tests/CMakeLists.txt` に追加します。

```cmake
add_gtest(test_my_new_device test_my_new_device.cpp)
```
