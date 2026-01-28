# 提案2: コールバック関数による疎結合化

この提案では、C++の `std::function` を活用して、オブジェクト間のハードな依存関係を取り除きます。**Observer パターン** の現代的なアプローチです。

## 1. クラス名の変更による役割の明確化

`CANManager` を **`CANDispatcher`** （CANディスパッチャー）に変更します。
これは、「受信したメッセージを適切なデバイスに配送（Dispatch）し、デバイスからの送信要求をドライバーに渡す」というハブとしての役割を強調します。

## 2. 循環参照の解消とデータフローの整理

`CANDevice` が特定の管理クラスへのポインタを持つ必要をなくします。代わりに、「どうやって送信するか」という振る舞い（関数）だけを受け取ります。

### クラス構造の変更

1. **`CANDevice`**
   - `CANManager` への参照を削除します。
   - コンストラクタで「送信用の関数オブジェクト」を受け取ります。
   
   ```cpp
   using SendCallback = std::function<bool(const CANFrame&)>;
   using ReceiveCallback = std::function<void(const CANFrame&)>;

   class CANDevice {
       SendCallback send_func_;
   public:
       // コンストラクタのみ。受信登録は外部（Dispatcher）が行う
       CANDevice(SendCallback send_func, ...) : send_func_(send_func) {}
       
       void do_task() {
           send_func_(frame); 
       }
       
       // 自身のメンバー関数として受信処理を持つ
       void on_receive(const CANFrame& frame) { ... }
   };
   ```

2. **`CANDispatcher` (旧 CANManager)**
   - デバイスオブジェクトそのものを管理するのではなく、**「ID」と「コールバック関数」の対応関係** を管理します。
   
   ```cpp
   #include <map>
   #include <functional>

   class CANDispatcher {
       using Callback = std::function<void(const CANFrame&)>;
       std::map<uint32_t, Callback> subscribers_;
       drivers::DriverInterface& driver_;

   public:
       // 特定のIDに対してコールバックを登録する
       void subscribe(uint32_t can_id, Callback callback) {
           subscribers_[can_id] = callback;
       }
       
       bool send_frame(const CANFrame& frame) {
           return driver_.send(frame);
       }

       void update() {
           CANFrame frame;
           while(driver_.receive(frame)) {
               // 登録されたIDがあればコールバックを実行
               if (subscribers_.count(frame.id)) {
                   subscribers_[frame.id](frame);
               }
           }
       }
   };
   ```
   
   **使用例 (main.cpp等):**
   ```cpp
   CANDispatcher dispatcher(driver);
   
   // 送信機能の注入
   auto send_impl = [&](const CANFrame& f) { return dispatcher.send_frame(f); };
   MotorDriver motor(send_impl, ...);
   
   // 受信機能の紐付け（購読）
   // モーターが受信すべきIDを指定して登録する
   dispatcher.subscribe(0x123, [&](const CANFrame& f) { motor.on_receive(f); });
   ```

## 3. メリット・デメリット

### メリット
- **最強の疎結合**: `CANDevice` は `CANDispatcher` の存在すら知る必要がありません。ヘッダーのインクルードも不要です。
- **柔軟性**: テスト時に、簡単なラムダ式を渡すだけで送信処理をモックできます。

### デメリット
- **デバッグの難易度**: コールバックの連鎖になるため、スタックトレースが追いづらくなる場合があります。
- **パフォーマンス**: `std::function` は関数ポインタに比べてわずかにオーバーヘッドがあります（通常は無視できるレベルです）。
