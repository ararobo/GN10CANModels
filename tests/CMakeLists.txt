cmake_minimum_required(VERSION 3.22)

if(BUILD_FOR_ROS2)
  if(BUILD_TESTING)
    find_package(ament_cmake_gtest REQUIRED)
    
    ament_add_gtest(test_can_frame test_can_frame.cpp)
    target_link_libraries(test_can_frame ${PROJECT_NAME})
    
    ament_add_gtest(test_can_converter test_can_converter.cpp)
    target_link_libraries(test_can_converter ${PROJECT_NAME})

    ament_add_gtest(test_can_bus test_can_bus.cpp)
    target_link_libraries(test_can_bus ${PROJECT_NAME})

    ament_add_gtest(test_motor_driver test_motor_driver.cpp)
    target_link_libraries(test_motor_driver ${PROJECT_NAME})
  endif()
else()
  enable_testing()

  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(test_can_frame test_can_frame.cpp)
  target_link_libraries(test_can_frame gtest_main ${PROJECT_NAME})
  
  add_executable(test_can_converter test_can_converter.cpp)
  target_link_libraries(test_can_converter gtest_main ${PROJECT_NAME})

  add_executable(test_can_bus test_can_bus.cpp)
  target_link_libraries(test_can_bus gtest_main ${PROJECT_NAME})

  add_executable(test_motor_driver test_motor_driver.cpp)
  target_link_libraries(test_motor_driver gtest_main ${PROJECT_NAME})

  include(GoogleTest)
  gtest_discover_tests(test_can_frame)
  gtest_discover_tests(test_can_converter)
  gtest_discover_tests(test_can_bus)
  gtest_discover_tests(test_motor_driver)
endif()
