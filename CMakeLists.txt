cmake_minimum_required(VERSION 3.8)
project(gn10_can_models)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compiler warnings
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
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
        
        # Add GTest integration here if needed later
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

endif()