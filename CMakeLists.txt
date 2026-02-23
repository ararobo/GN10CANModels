cmake_minimum_required(VERSION 3.22)
project(gn10_can VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
  add_compile_options(/W4)
endif()

set(SOURCES
    src/core/can_bus.cpp
    src/devices/motor_driver_types.cpp
    src/devices/motor_driver_client.cpp
    src/devices/motor_driver_server.cpp
)

# Option to enable STM32 drivers (Requires HAL headers)
option(ENABLE_STM32_DRIVERS "Build STM32 drivers (requires HAL)" OFF)

if(ENABLE_STM32_DRIVERS)
    list(APPEND SOURCES
        drivers/stm32_can/driver_stm32_can.cpp
        drivers/stm32_fdcan/driver_stm32_fdcan.cpp
    )
endif()

find_package(ament_cmake QUIET)
find_package(ament_cmake_auto QUIET)

option(BUILD_FOR_ROS2 "Build with ament_cmake for ROS 2" ${ament_cmake_FOUND})

if(BUILD_FOR_ROS2)
    if(NOT ament_cmake_FOUND)
        message(FATAL_ERROR "BUILD_FOR_ROS2 is ON but ament_cmake was not found.")
    endif()

    message(STATUS "Building for ROS2")

    ament_auto_add_library(${PROJECT_NAME} ${SOURCES})

    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    ament_export_include_directories(include)
    ament_export_libraries(${PROJECT_NAME})

    install(DIRECTORY include/ DESTINATION include)

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )

    if(BUILD_TESTING)
        find_package(ament_lint_auto REQUIRED)
        add_subdirectory(tests)
    endif()

    ament_package()
else()
    message(STATUS "Building for Generic C++")

    add_library(${PROJECT_NAME} STATIC ${SOURCES})

    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    # STM32 ドライバのヘッダを公開 (HAL ヘッダは利用側が提供する)
    if(ENABLE_STM32_DRIVERS)
        target_include_directories(${PROJECT_NAME} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )
    endif()

    install(TARGETS ${PROJECT_NAME} DESTINATION lib)
    install(DIRECTORY include/ DESTINATION include)

    option(BUILD_TESTS "Build tests" OFF)
    if(BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()
endif()
