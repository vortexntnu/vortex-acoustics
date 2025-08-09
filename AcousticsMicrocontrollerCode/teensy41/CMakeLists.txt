cmake_minimum_required(VERSION 3.20)

# Avoid linking during try-compile on bare metal
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Point to the SDK subfolder that has 'cmake/toolchain/armgcc.cmake'
# (/home/.../mcuxpresso-sdk/mcuxsdk)
set(SDK_ROOT "$ENV{HOME}/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk" CACHE PATH "MCUXpresso SDK root")
set(SdkRootDirPath "$ENV{HOME}/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk" CACHE PATH "MCUXpresso SDK root")

project(teensy41_bare C ASM)

# Bring in MCUX helper macros (defines mcux_add_cmakelists, etc.)
include(${SdkRootDirPath}/cmake/toolchain/mcux.cmake)
include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

# Device tuple for i.MX RT1062:
set(device_root   "devices")
set(soc_portfolio "RT")
set(soc_series    "RT1060")
set(device        "MIMXRT1062")

# CPU/flags (you can tweak; toolchain will add its own defaults too)
set(MCPU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")
add_compile_options(${MCPU_FLAGS} -ffunction-sections -fdata-sections -O2)
add_link_options(${MCPU_FLAGS} -Wl,--gc-sections -Wl,-Map=${CMAKE_PROJECT_NAME}.map)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === SELECTIVE IMPORTS from the SDK ===
# 1) Device (headers, startup, system)
mcux_add_cmakelists(${SDK_ROOT}/${device_root}/${soc_portfolio}/${soc_series}/${device})

# 2) Drivers you use
mcux_add_cmakelists(${SDK_ROOT}/drivers/common)
mcux_add_cmakelists(${SDK_ROOT}/drivers/igpio)
mcux_add_cmakelists(${SDK_ROOT}/drivers/lpuart)
mcux_add_cmakelists(${SDK_ROOT}/drivers/flexcan)

# (OPTIONAL) CMSIS
mcux_add_cmakelists(${SDK_ROOT}/arch/arm/CMSIS)

# === Your application ===
add_executable(${PROJECT_NAME}.elf
  src/main.c
  src/dsp.c
  src/adc.c
  src/pit.c
  src/clock.c
  src/multilateration.c
)

# Link with the imported device/driver targets (names can vary by SDK release)
target_link_libraries(${PROJECT_NAME}.elf PRIVATE
  device_MIMXRT1062_CMSIS
  device_MIMXRT1062_system
  device_MIMXRT1062_startup
  driver_common
  driver_clock
  driver_iomuxc
  driver_igpio
  driver_lpuart
  driver_flexcan
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Linker script (FlexSPI NOR; adjust if needed)
set(LD_DIR "${SDK_ROOT}/${device_root}/${soc_portfolio}/${soc_series}/${device}/gcc")
if(EXISTS "${LD_DIR}/MIMXRT1062xxxxx_flexspi_nor.ld")
  target_link_options(${PROJECT_NAME}.elf PRIVATE "-T${LD_DIR}/MIMXRT1062xxxxx_flexspi_nor.ld")
elseif(EXISTS "${LD_DIR}/i_nor.ld")
  target_link_options(${PROJECT_NAME}.elf PRIVATE "-T${LD_DIR}/i_nor.ld")
else()
  message(FATAL_ERROR "No suitable linker script found in ${LD_DIR}")
endif()

# Size + bin/hex after link
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_SIZE    arm-none-eabi-size    REQUIRED)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
  COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
)


