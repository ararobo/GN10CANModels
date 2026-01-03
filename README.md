# GN10 CAN Library
CAN bus data models, ID definitions, and handling classes.

This repository is started by @aiba-gento for the robotics communication ecosystem.

## Supported Platforms

This library is designed to work across multiple platforms:

- **ESP32** - Arduino environment
- **STM32** - Makefile/CubeIDE
- **ROS 2** - CMake/Linux

## Project Structure
```text
gn10-can/
├── include/gn10_can/ # Header files (API definitions)
├── src/                     # Implementation files
├── tests/                   # Unit tests (GTest)
├── uml/                     # UML diagrams
└── CMakeLists.txt           # Build configuration
```

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
- Files under `include/gn10_can_models/` must **NOT** include platform-specific headers such as `<Arduino.h>`, `<rclcpp/rclcpp.h>`, or `<hal_driver.h>`.
- Only standard C++ headers are allowed: `<cstdint>`, `<cstring>`, `<cmath>`, `<algorithm>`, etc.

### 4. Hardware Abstraction
- This library provides **data packing (serialize) / unpacking (deserialize) functions only**.
- CAN transmission/reception functions (drivers) are **NOT** included in this library.
- Actual CAN bus interaction must be implemented in the platform-specific application layer.

### 5. Data Representation (Endianness)
- Unless otherwise specified in a specific model, multi-byte data is packed in **Little-Endian (Intel format)** by default.
- **Note**: The implementation relies on the host CPU's endianness (typically Little-Endian on ARM/x86).

### 6. Memory Management (No Dynamic Allocation)
- To ensure stability on embedded systems (STM32/ESP32), avoid using dynamic memory allocation (`new`, `malloc`, `std::vector`, `std::string`) inside the models.
- Use fixed-size arrays and primitive types.

### 7. Language Policy
- **Code & Commits**: English.
- **Documentation**: English (Japanese translation allowed).
- **Internal Comments**: English preferred, but Japanese is allowed for complex logic explanations.

## License
This project is licensed under the GNU General Public License v3.0 (GPL-3.0) - see the [LICENSE](LICENSE) file for details.