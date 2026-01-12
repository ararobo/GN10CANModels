set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_program(CMAKE_C_COMPILER "arm-none-eabi-gcc")
find_program(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
find_program(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")

find_program(CMAKE_OBJCOPY "arm-none-eabi-objcopy")
find_program(CMAKE_SIZE "arm-none-eabi-size")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
