/**
 * @file motor_driver_types.hpp
 * @author Gento Aiba (aiba-gento)
 * @brief モータードライバー関連の型定義ヘッダーファイル
 * @version 0.1.0
 * @date 2026-01-10
 *
 * @copyright Copyright (c) 2026 Gento Aiba
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

namespace gn10_can {
namespace devices {

/**
 * @brief エンコーダータイプの定義
 */
enum class EncoderType : uint8_t {
    None             = 0,  ///< @brief エンコーダーなし
    IncrementalSpeed = 1,  ///< @brief インクリメンタルエンコーダー (速度制御)
    Absolute         = 2,  ///< @brief アブソリュートエンコーダー
    IncrementalTotal = 3,  ///< @brief インクリメンタルエンコーダー (位置制御)
};

/**
 * @brief ゲインタイプの定義
 */
enum class GainType : uint8_t {
    Kp    = 0x00,  ///< @brief 比例ゲイン (Proportional)
    Ki    = 0x01,  ///< @brief 積分ゲイン (Integral)
    Kd    = 0x02,  ///< @brief 微分ゲイン (Derivative)
    Ff    = 0x03,  ///< @brief フィードフォワードゲイン (Feed Forward)
    Count = 0x04   ///< @brief ゲインタイプの総数
};

/**
 * @brief モータードライバーの設定データを管理するクラス
 */
class MotorConfig
{
public:
    MotorConfig() = default;

    /**
     * @brief 最大duty比を制限する (0.0 - 1.0)
     * @param ratio 0.0 (0%) から 1.0 (100%) の範囲の浮動小数点値
     */
    void set_max_duty_ratio(float ratio);

    /**
     * @brief 設定された最大duty比を取得する
     * @return float 最大duty比 (0.0 - 1.0)
     */
    float get_max_duty_ratio() const;

    /**
     * @brief 最大加速度を設定する
     * @param ratio 0.0 (最も遅い) から 1.0 (最も速い/即時)
     */
    void set_accel_ratio(float ratio);

    /**
     * @brief 設定された最大加速度を取得する
     * @return float 最大加速度 (0.0 - 1.0)
     */
    float get_accel_ratio() const;

    /**
     * @brief 正転側のリミットスイッチを設定する
     * @param enable_stop リミットスイッチ検知時に停止するかどうか
     * @param switch_id 正転側リミットスイッチのID (0-7)
     */
    void set_forward_limit_switch(bool enable_stop, uint8_t switch_id);

    /**
     * @brief 設定された正転側リミットスイッチ設定を取得する
     * @param[out] enable_stop 停止有効フラグ
     * @param[out] switch_id スイッチID
     */
    void get_forward_limit_switch(bool& enable_stop, uint8_t& switch_id) const;

    /**
     * @brief 逆転側のリミットスイッチを設定する
     * @param enable_stop リミットスイッチ検知時に停止するかどうか
     * @param switch_id 逆転側リミットスイッチのID (0-7)
     */
    void set_reverse_limit_switch(bool enable_stop, uint8_t switch_id);

    /**
     * @brief 設定された逆転側リミットスイッチ設定を取得する
     * @param[out] enable_stop 停止有効フラグ
     * @param[out] switch_id スイッチID
     */
    void get_reverse_limit_switch(bool& enable_stop, uint8_t& switch_id) const;

    /**
     * @brief フィードバック送信周期を設定する
     * @param ms 送信周期 (ミリ秒)
     */
    void set_feedback_cycle(uint8_t ms);

    /**
     * @brief 設定されたフィードバック送信周期を取得する
     * @return uint8_t 送信周期 (ミリ秒)
     */
    uint8_t get_feedback_cycle() const;

    /**
     * @brief エンコーダータイプを設定する
     * @param type エンコーダータイプ
     */
    void set_encoder_type(EncoderType type);

    /**
     * @brief 設定されたエンコーダータイプを取得する
     * @return EncoderType エンコーダータイプ
     */
    EncoderType get_encoder_type() const;

    /**
     * @brief ユーザーオプションを設定する
     * @param option ユーザーオプション値
     */
    void set_user_option(uint8_t option);

    /**
     * @brief 設定されたユーザーオプションを取得する
     * @return uint8_t ユーザーオプション値
     */
    uint8_t get_user_option() const;

    /**
     * @brief 設定データをバイト配列にシリアライズする
     * @return std::array<uint8_t, 8> CANフレームのペイロード
     */
    std::array<uint8_t, 8> to_bytes() const;

    /**
     * @brief バイト配列から設定データを復元する
     * @param bytes CANフレームのペイロード
     * @return MotorConfig 復元された設定データ
     */
    static MotorConfig from_bytes(const std::array<uint8_t, 8>& bytes);

private:
    static uint8_t map_ratio_to_u8(float ratio);

#ifdef _MSC_VER
#pragma pack(push, 1)
    struct PackedData {
#else
    struct __attribute__((__packed__)) PackedData {
#endif
        /**
         * @brief 最大出力制限 (正規化済み)
         * 範囲: 0 (0%) から 255 (100%)
         */
        uint8_t max_duty_ratio;
        /**
         * @brief 最大加速度 (ランプレート)
         * 単位: 1ミリ秒あたりの正規化されたデューティ変化量
         * 範囲: 0 から 255
         * 例: 値が5の場合、1ミリ秒あたり約2% (5/255) デューティが変化する
         */
        uint8_t max_accel_rate;
        /**
         * @brief フィードバック送信周期
         * 単位: ミリ秒 (ms)
         * 0の場合、フィードバックは無効
         */
        uint8_t feedback_cycle_ms;
        /**
         * @brief エンコーダータイプ設定
         * EncoderType 列挙型を参照
         */
        uint8_t encoder_type;
        /**
         * @brief リミットスイッチ設定フラグ
         * ビットフィールド: [StopFwd(1) | FwdID(3) | StopBwd(1) | BwdID(3)]
         */
        uint8_t limit_switches_config;
        uint8_t user_option;
        uint8_t reserved[2];  // 8byteで揃えたい
    };
#ifdef _MSC_VER
#pragma pack(pop)
#endif

    PackedData data_{};
};

}  // namespace devices
}  // namespace gn10_can