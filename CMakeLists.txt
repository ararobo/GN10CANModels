cmake_minimum_required(VERSION 3.8)
project(gn10_can)

# Default to C++14 (C++17 is also fine, but 14 is safer for older embedded compilers)
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
  add_compile_options(/W4)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

find_package(ament_cmake QUIET)

# Allow users to force non-ROS build even if ament_cmake is present
option(BUILD_FOR_ROS2 "Build with ament_cmake for ROS 2" ${ament_cmake_FOUND})

# Conditional build for ROS2 or generic C++
if(BUILD_FOR_ROS2)
    if(NOT ament_cmake_FOUND)
        message(FATAL_ERROR "BUILD_FOR_ROS2 is ON but ament_cmake was not found.")
    endif()

    message(STATUS "Building for ROS2")

    ament_auto_add_library(${PROJECT_NAME}
        ${SOURCES}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    ament_export_include_directories(include)
    ament_export_libraries(${PROJECT_NAME})
  
    install(
        DIRECTORY include/
        DESTINATION include
    )

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )

    if(BUILD_TESTING)
        find_package(ament_lint_auto REQUIRED)
        ament_lint_auto_find_test_dependencies()
        
        add_subdirectory(tests)
    endif()

    ament_package()
    
else()
    message(STATUS "Building for Generic C++")

    add_library(${PROJECT_NAME} STATIC
        ${SOURCES}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    # Basic installation support for generic CMake users
    install(TARGETS ${PROJECT_NAME} DESTINATION lib)
    install(DIRECTORY include/ DESTINATION include)

    option(BUILD_TESTS "Build tests" ON)
    if(BUILD_TESTS)
        add_subdirectory(tests)
    endif()

endif()