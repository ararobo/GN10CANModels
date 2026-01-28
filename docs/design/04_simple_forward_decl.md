# 提案4: 前方宣言を活用したシンプル構成

この提案では、新しいクラスや複雑なパターンを導入せず、C++の基本機能である **前方宣言 (Forward Declaration)** を活用して循環参照を解決します。
また、効率化のためにマネージャー側にID登録型のルーティング機能を追加します。

## 1. 循環参照の解消

ヘッダーファイルでの `#include` を最小限にし、クラスの内部実装（.cpp）でのみ相互の定義を参照するようにします。

## 2. クラス構造の改善

既存のクラス名（`CANManager`, `CANDevice`）はそのまま利用しつつ、内部構造を整理します。

### 変更点

1.  **`CANDevice` (ヘッダー)**
    - `CANManager` をインクルードせず、`class CANManager;` と前方宣言のみ行います。
    - ポインタとして参照を保持します。

2.  **`CANManager` (ヘッダー)**
    - `CANDevice` をインクルードしません（デバイスの具体を知る必要がないため、ベースクラスがあれば十分、あるいは前方宣言）。
    - 受信IDとデバイスポインタのマップ `std::map<uint32_t, CANDevice*>` を持ちます。

### コード例

**`can_device.hpp`**
```cpp
#pragma once
#include "gn10_can/core/can_frame.hpp"

namespace gn10_can {

// 前方宣言: ヘッダー内でこのクラスの中身を知る必要はない
class CANManager;

class CANDevice {
protected:
    CANManager* manager_; // 参照ではなくポインタ推奨（前方宣言と相性が良い）
    uint32_t my_id_;

public:
    CANDevice(CANManager* manager, uint32_t id) 
        : manager_(manager), my_id_(id) {}

    virtual ~CANDevice() = default;

    // 自分宛てのメッセージのみ届く
    virtual void on_receive(const CANFrame& frame) = 0;
    
    // 送信処理（実装はcppファイルに記述）
    void send(const CANFrame& frame); 

    uint32_t get_id() const { return my_id_; }
};
}
```

**`can_manager.hpp`**
```cpp
#pragma once
#include <map>
#include "gn10_can/drivers/driver_interface.hpp"

namespace gn10_can {

class CANDevice; // 前方宣言

class CANManager {
    std::map<uint32_t, CANDevice*> subscribers_;
    drivers::DriverInterface& driver_;

public:
    CANManager(drivers::DriverInterface& driver) : driver_(driver) {}

    // IDを指定して登録（デバイス内のIDを使うなら引数減らせる）
    void register_device(CANDevice* device);

    bool send_frame(const CANFrame& frame) {
        return driver_.send(frame);
    }

    void update();
};
}
```

**`can_device.cpp`** (実装ファイル)
```cpp
#include "gn10_can/core/can_device.hpp"
#include "gn10_can/core/can_manager.hpp" // ここで初めてManagerの中身を知る

namespace gn10_can {
    void CANDevice::send(const CANFrame& frame) {
        if (manager_) {
            manager_->send_frame(frame);
        }
    }
}
```

## 3. 使用例 (main.cpp)

```cpp
#include "gn10_can/core/can_manager.hpp"
#include "gn10_can/drivers/stm32_can/driver_stm32_can.hpp"
#include "my_specific_devices.hpp" // 具体的なモータードライバーなど

int main() {
    // 1. ドライバーとマネージャーの初期化
    gn10_can::drivers::Stm32CanDriver driver;
    gn10_can::CANManager manager(driver);

    // 2. デバイス作成（マネージャーへのポインタを渡す）
    // ID: 0x100
    MotorDriver motor1(&manager, 0x100);
    
    // 3. マネージャーに登録（このID宛のデータがmotor1に届くようになる）
    manager.register_device(&motor1);

    while (true) {
        // 4. メインループで更新
        manager.update();
    }
}
```

## 4. メリット・デメリット

### メリット
- **最もシンプル**: 新しい概念（Interfaceクラス、Portクラス、std::function）を覚える必要がありません。
- **既存コードへの親和性**: 現在のコードからの変更量が最小で済みます。
- **コンパイル時間の短縮**: ヘッダーの依存関係が切れるため、変更時の再コンパイル範囲が狭まります。

### デメリット
- **密結合**: 依然として `CANDevice` は `CANManager` というクラスの実装（send_frameメソッドがあること）に依存しています。テスト時に `CANManager` のモックを作るのが少し大変かもしれません。
