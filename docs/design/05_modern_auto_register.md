# 提案5: RAIIによる自動登録とモダンAPI

この提案は、**「ユーザーがいかに少ないコード量で、ミスなく安全に実装できるか」**（Developer Experience）を最優先した設計です。
RAII (Resource Acquisition Is Initialization) パターンを活用した自動登録と、直感的なAPIを提供します。

## 1. コンセプト: "It just works"

これまでの提案でユーザーにとって手間だった「マネージャーへの手動登録」を廃止します。
デバイスのインスタンスを作成するだけで、自動的に通信ネットワークに参加できるようにします。

## 2. クラス設計の刷新

### 2.1 `CANBus` (旧 CANManager)

管理クラスをより直感的な `CANBus` という名前にします。これは物理的な「バス」をソフトウェア上で表現したものです。

```cpp
class CANBus {
    // 受信IDをキーにしたルーティングテーブル
    std::map<uint32_t, CANDevice*> devices_;
    drivers::DriverInterface& driver_;
    
    // ユーザーは直接呼ばない（CANDeviceが呼ぶ）
    friend class CANDevice;
    void attach(CANDevice* device, uint32_t rx_id);
    void detach(uint32_t rx_id);

public:
    CANBus(drivers::DriverInterface& driver) : driver_(driver) {}
    
    // バス全体の更新
    void update();
    
    // 生のフレーム送信（基本はDevice経由で行うが、デバッグ用に公開）
    bool send_raw(const CANFrame& frame);
};
```

### 2.2 `CANDevice` (自動登録基底クラス)

このクラスを継承するだけで、登録解除処理が自動化されます。

```cpp
class CANDevice {
protected:
    CANBus& bus_;
    uint32_t my_id_; // 受信ID

public:
    // コンストラクタで自動登録 (Attach)
    CANDevice(CANBus& bus, uint32_t id) : bus_(bus), my_id_(id) {
        bus_.attach(this, my_id_);
    }

    // デストラクタで自動解除 (Detach)
    virtual ~CANDevice() {
        bus_.detach(my_id_);
    }
    
    // 【改善点】オーバーロードによる直感的な送信API
    // 例: データのみ送信
    bool send(const std::vector<uint8_t>& data);
    
    // 例: コマンド付き送信
    bool send(uint8_t cmd, const std::vector<uint8_t>& data);

    // 受信ハンドラ（純粋仮想関数）
    virtual void on_receive(const CANFrame& frame) = 0;
};
```

## 3. 使用例: 驚くほどシンプルに

ユーザーが書くコード（main.cpp）は劇的に簡素化されます。

```cpp
#include "gn10_can/core/can_bus.hpp"
#include "my_devices.hpp"

// ユーザー定義のモータークラス
class MyMotor : public gn10_can::CANDevice {
public:
    // 親クラスのコンストラクタを呼ぶだけでセットアップ完了
    MyMotor(gn10_can::CANBus& bus, uint32_t id) 
        : CANDevice(bus, id) {}

    void on_receive(const gn10_can::CANFrame& frame) override {
        // IDフィルタリング済み。自分宛てのデータだけが来る。
        // ...処理...
    }
    
    void set_speed(int16_t speed) {
        // 直感的なデータ作成。面倒なCANFrameの構築は隠蔽。
        std::vector<uint8_t> data = { 
            static_cast<uint8_t>(speed >> 8), 
            static_cast<uint8_t>(speed & 0xFF) 
        };
        this->send(0x01, data); // cmd=0x01 で送信
    }
};

int main() {
    gn10_can::drivers::Stm32CanDriver driver;
    gn10_can::CANBus bus(driver);

    // インスタンス化するだけで通信可能！
    // register_device() の呼び出し忘れバグが発生しない
    MyMotor motor_left(bus, 0x100);
    MyMotor motor_right(bus, 0x101);

    while (true) {
        bus.update();
        
        motor_left.set_speed(100);
    }
}
```

## 4. メリット・デメリット

### メリット
- **使いやすさNo.1**: 初期化コードが最小限で済みます。
- **安全性 (RAII)**: インスタンスがスコープアウトして削除されると、自動的にバスからも登録抹消されるため、ダングリングポインタ（無効なポインタへのアクセス）を防げます。
- **APIの隠蔽**: 複雑な `CANFrame` 構造体操作をユーザーに見せず、`send({1, 2, 3})` のように書けます。

### デメリット
- **隠れた依存性**: 「コンストラクタで副作用（登録処理）が発生する」ことが見えにくいため、純粋な単体テストを行う際にMock Busが必要になります。
