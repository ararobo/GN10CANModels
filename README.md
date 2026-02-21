# GN10 CAN Library
[![CI](https://github.com/ararobo/gn10-can/actions/workflows/test.yml/badge.svg)](https://github.com/ararobo/gn10-can/actions/workflows/test.yml)
![Platform](https://img.shields.io/badge/Platform-STM32%20|%20ESP32%20|%20ROS2-blue)

[日本語 (Japanese)](README_ja.md)

CAN bus data models, ID definitions, and handling classes.

Primarily intended for robot contests, this library simplifies firmware development for custom PCBs by centralizing CAN data frame and ID definitions. It aims to reduce configuration effort while enhancing reliability, reproducibility, and development efficiency.

## Supported Platforms

This library is designed to work across multiple platforms:

- **ESP32** - Arduino environment
- **STM32** - CMake
- **ROS 2** - CMake/Linux

## Development Environment Setup

### Common
Install **CMake Tools** extension for VSCode.

### Ubuntu
```bash
sudo apt update
sudo apt install build-essential cmake ninja-build
```

### Windows(for STM32)

Install STM32CubeCLT.

### Windows(Generic)

Install a C++ Compiler(Visual Studio or MinGW), CMake, and Ninja, and add them to your PATH.

## Integration

### CMake (`FetchContent` or Submodule)
Add the following to your `CMakeLists.txt`:
```cmake
# Example: If placed under 'libs' folder
add_subdirectory(libs/gn10-can)
target_link_libraries(<your_target_name> PRIVATE gn10_can)
```

## Build

### Generic C++ (CMake)

```bash
mkdir build && cd build
cmake -DBUILD_FOR_ROS2=OFF .. # -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
test
```bash
mkdir build && cd build
cmake -DBUILD_FOR_ROS2=OFF -DBUILD_TESTS=ON .. # -DCMAKE_BUILD_TYPE=Release
cmake --build .
ctest  # Run tests
```

### ROS 2 (Colcon)

```bash
colcon build --packages-select gn10_can
```
test
```bash
colcon test --packages-select gn10_can
colcon test-result --all
```

## Usage

### 1. Implement Driver Interface
You need to implement `gn10_can::drivers::DriverInterface` for your specific hardware (e.g., STM32, ESP32 , SocketCAN, etc.).

```cpp
#include "gn10_can/drivers/driver_interface.hpp"

class MyCANDriver : public gn10_can::drivers::DriverInterface {
public:
    bool send(const gn10_can::CANFrame& frame) override {
        // Implement hardware send
        return true;
    }
    bool receive(gn10_can::CANFrame& out_frame) override {
        // Implement hardware receive
        return true;
    }
};
```

### 2. Setup Bus and Devices

```cpp
#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/devices/motor_driver_client.hpp"
#include "gn10_can/devices/solenoid_driver_client.hpp"

// ... inside your main loop or setup ...

MyCANDriver driver;
// 1. Initialize Bus directly with the driver
gn10_can::CANBus bus(driver);

// 2. Initialize Devices
// RAII: Devices automatically attach to the Bus on construction
// and detach on destruction. No manual registration needed.
gn10_can::devices::MotorDriverClient motor(bus, 0);       // Motor Driver (ID: 0)
gn10_can::devices::SolenoidDriverClient solenoid(bus, 1); // Solenoid Driver (ID: 1)

// Send commands
motor.set_target(100.0f); // Set target velocity/position to Motor
solenoid.set_target(true); // Turn on Solenoid

// Main loop
while (true) {
    // Process incoming messages.
    // (Note: You can also call bus.update() directly in the CAN receive
    //  interrupt or in the driver's receive callback function for lower latency.)
    bus.update();

    // ... your application logic ...
}
```

## Project Structure
```text
gn10-can/
├── include/gn10_can/
│   ├── core/        # Core logic (Bus, Device base, Frame)
│   ├── devices/     # Device implementations (MotorDriver, etc.)
│   ├── drivers/     # Hardware interfaces
│   └── utils/       # Utilities (Converter, etc.)
├── src/             # Implementation files
├── tests/           # Unit tests (GTest)
├── uml/             # UML diagrams
└── CMakeLists.txt   # Build configuration
```

## Class Diagram
Class diagram (simplified)

![Class diagram simplified](uml/class_diagram_simplified.png)

[Class diagram detail](uml/class_diagram.png)

## Development Rules

### 1. Naming Conventions
Variable and function names must be self-explanatory. We follow the **Google C++ Style Guide** basics:
- **Class/Struct names**: `PascalCase` (e.g., `SpeedMsg`, `BatteryStatus`)
- **Function/Variable names**: `snake_case` (e.g., `get_id()`, `target_velocity`)
- **Constants/Enum values**: `kPascalCase` or `ALL_CAPS` (e.g., `kMaxSpeed`, `BATTERY_LOW`)
- **Private Member variables**: Must have a trailing underscore (e.g., speed_, voltage_) to distinguish them from local variables.

### 2. Code Formatting
- Do not rely on IDE defaults.
- All code must be formatted using **Clang-Format**.
- A `.clang-format` file is provided in the root directory. Please configure your editor to use it on save.

### 3. Standard Library Only
To ensure cross-platform compatibility:
- Files under `include/gn10_can/` must **NOT** include platform-specific headers such as `<Arduino.h>`, `<rclcpp/rclcpp.h>`, or `<hal_driver.h>`.
- Only standard C++ headers are allowed: `<cstdint>`, `<cstring>`, `<cmath>`, `<algorithm>`, etc.

### 4. Memory Management (No Dynamic Allocation)
- To ensure stability on embedded systems (STM32/ESP32), avoid using dynamic memory allocation (`new`, `malloc`, `std::vector`, `std::string`) inside the models.
- Use fixed-size arrays and primitive types.

### 5. Language Policy
- **Code & Commits**: English.
- **Documentation**: English (Japanese translation allowed).
- **Internal Comments**: Japanese

## License
This project is licensed under the Apache-2.0 - see the [LICENSE](LICENSE) file for details.

## Maintainer
- Gento Aiba
- Koichiro Watanabe
- Ayu Kanai
- Akeru Kamagata
