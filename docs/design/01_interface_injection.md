# 提案1: インターフェース分離による依存関係の逆転

この提案では、**依存関係逆転の原則 (DIP)** を適用し、各クラスが具体的な実装ではなく抽象インターフェースに依存するように変更します。これにより循環参照を解決し、責務を明確にします。

## 1. クラス名の変更による役割の明確化

`CANManager` という抽象的な名前を **`CANBusController`** に変更します。
これは「CANバス上の通信フローを制御（Control）する役割」であることを明確にします。

## 2. 循環参照の解消とデータフローの整理

現在の設計では、`CANDevice` が `CANManager` を知り、`CANManager` が `CANDevice` を知っている状態です。これを解決するために、送信と受信のインターフェースを分離します。

### 新しいインターフェース定義

```cpp
// 1. 送信機能のインターフェース
class ICANSender {
public:
    virtual bool send_frame(const CANFrame& frame) = 0;
    virtual ~ICANSender() = default;
};

// 2. 受信機能のインターフェース
class ICANReceiver {
public:
    // 自分宛てのメッセージのみが届くため、IDチェックは不要
    virtual void on_receive(const CANFrame& frame) = 0;
    
    // 受信したいCAN-ID（フィルター）を返す必要がある
    virtual uint32_t get_target_id() const = 0;
    
    virtual ~ICANReceiver() = default;
};
```

### クラス構造の変更

1. **`CANDevice`**
   - `CANManager` そのものではなく、より限定的な **`ICANSender`** インターフェースに依存します。
   - 自身は **`ICANReceiver`** を実装します。
   
   ```cpp
   class CANDevice : public ICANReceiver {
       ICANSender& sender_; 
       uint32_t my_id_; // 自身のID
   public:
       CANDevice(ICANSender& sender, uint32_t id, ...) : sender_(sender), my_id_(id) {}
       
       void do_something() {
           sender_.send_frame(frame);
       }
       
       // ICANReceiverの実装
       uint32_t get_target_id() const override { return my_id_; }
       
       void on_receive(const CANFrame& frame) override {
           // ここに来る時点で自分宛てであることが保証される
           process(frame); 
       }
   };
   ```

2. **`CANBusController` (旧 CANManager)**
   - **`ICANSender`** を実装します。
   - 受信IDをキーとしたマップ、またはハッシュテーブルでレシーバーを管理し、適切な相手にのみイベントを配送します。
   
   ```cpp
   #include <map>

   class CANBusController : public ICANSender {
       // IDごとに受信者を振り分けるルーティングテーブル
       std::map<uint32_t, ICANReceiver*> router_;
       drivers::DriverInterface& driver_;
   public:
       void register_receiver(ICANReceiver* receiver) {
           router_[receiver->get_target_id()] = receiver;
       }

       bool send_frame(const CANFrame& frame) override {
           return driver_.send(frame);
       }
       
       void update() {
           CANFrame frame;
           while(driver_.receive(frame)) {
               // IDに基づいて適切なレシーバーを検索
               auto it = router_.find(frame.id);
               if (it != router_.end()) {
                   it->second->on_receive(frame);
               }
           }
       }
   };
   ```

## 3. メリット・デメリット

### メリット
- **循環参照の完全な解消**: ヘッダーファイルレベルでの相互インクルードが不要になります。
- **テスタビリティの向上**: `ICANSender` のモックを作ることで、 `CANDevice` 単体のテストが容易になります。
- **責務の分離**: 送信と受信の契約がインターフェースとして明示されます。

### デメリット
- インターフェースクラス（純粋仮想クラス）が増えるため、ファイル数やコード記述量が若干増加します。

## 4. 使用例 (main.cpp)

```cpp
#include "gn10_can/core/can_bus_controller.hpp" // 旧 CANManager
#include "gn10_can/drivers/stm32_can/driver_stm32_can.hpp"
#include "my_motor_driver.hpp"

int main() {
    // 1. ドライバー初期化
    gn10_can::drivers::Stm32CanDriver driver;
    
    // 2. コントローラー（Sender実装）初期化
    gn10_can::CANBusController controller(driver);

    // 3. デバイス作成
    // CANDeviceは ICANSender インターフェースだけを知っていれば良い
    // ID: 0x200 を受信IDとして持つとする
    MyMotorDriver motor(controller, 0x200);

    // 4. 受信登録
    // controllerは motorを ICANReceiver として扱う
    controller.register_receiver(&motor);

    while (true) {
        controller.update();
    }
}
```
