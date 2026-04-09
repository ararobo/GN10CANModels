#pragma once

namespace gn10_can {
namespace devices {
struct ESCHubConfig {
    float kp = 0.0f;
    float ki = 0.0f;
    float kd = 0.0f;
    float ff = 0.0f;

    void set_gain_kp(float kp);

    void set_gain_ki(float ki);

    void set_gain_kd(float kd);

    void set_gain_ff(float ff);

    float get_gain_kp();

    float get_gain_ki();

    float get_gain_kd();

    float get_gain_ff();
};
}  // namespace devices

}  // namespace gn10_can
