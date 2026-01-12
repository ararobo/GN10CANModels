# システムの種類を「Generic（マイコン/ベアメタル）」にする
# これを書くことで、Windows用のライブラリ（kernel32など）がリンクされなくなります
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# コンパイラの強制指定（STM32CubeCLTのパス）
# set(CMAKE_C_COMPILER "C:/ST/STM32CubeCLT_1.20.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc.exe")
# set(CMAKE_CXX_COMPILER "C:/ST/STM32CubeCLT_1.20.0/GNU-tools-for-STM32/bin/arm-none-eabi-g++.exe")
# set(CMAKE_ASM_COMPILER "C:/ST/STM32CubeCLT_1.20.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc.exe")

# マイコンはリンカスクリプトがないとexe化できないため、
# チェック段階では「ライブラリ作成ができるか」だけを確認させます。
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)