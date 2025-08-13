cmake_minimum_required(VERSION 3.22)

# Avoid hosted link during try-compile on bare metal
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# find_package(Mcuxsdk REQUIRED)

# --- EDIT: your SDK "mcuxsdk" root (has cmake/extension/mcux.cmake) ---
set(SdkRootDirPath "$ENV{HOME}/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk" CACHE PATH "")

# Pull in NXP CMake extensions (defines mcux_add_source/mcux_add_include/etc.)
include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

# Device-only project (no board path)
project(teensy41 LANGUAGES C ASM)

# Tell the SDK which device/core/toolchain you're targeting.
# (We pass them on the cmake command line too; setting here helps for IDEs.)
set(device  "MIMXRT1062"   CACHE STRING "" FORCE)
set(core_id "cm7"           CACHE STRING "" FORCE)
set(CONFIG_TOOLCHAIN "armgcc" CACHE STRING "" FORCE)

# Bring in the SDK’s top-level CMake so device/drivers can be found
include(${SdkRootDirPath}/CMakeLists.txt)

# ---- Your sources / includes (extension-style) ----
# Use your own project root as BASE_PATH for convenience
mcux_add_include(
  BASE_PATH ${CMAKE_SOURCE_DIR}
  INCLUDES include
)

mcux_add_source(
  BASE_PATH ${CMAKE_SOURCE_DIR}
  SOURCES src/main.c
          src/dsp.c
          src/adc.c
          src/pit.c
          src/clock.c
          src/multilateration.c
)

# CPU flags + newlib stubs for bare-metal
# mcux_add_macro(
#   CC "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -O2"
# )
# mcux_add_macro(
#   LD "-Wl,--gc-sections -Wl,-Map=${MCUX_SDK_PROJECT_NAME}.map --specs=nosys.specs --specs=nano.specs"
# )

# Linker script (FlexSPI NOR). Adjust if your SDK uses a different name.
# set(_ld_dir "${SdkRootDirPath}/devices/RT/RT1060/MIMXRT1062/gcc")
# if(EXISTS "${_ld_dir}/MIMXRT1062xxxxx_flexspi_nor.ld")
#   mcux_add_macro(LD "-T${_ld_dir}/MIMXRT1062xxxxx_flexspi_nor.ld")
# elseif(EXISTS "${_ld_dir}/i_nor.ld")
#   mcux_add_macro(LD "-T${_ld_dir}/i_nor.ld")
# else()
#   message(FATAL_ERROR "No suitable linker script found in ${_ld_dir}")
# endif()

# Optional: extra driver/source globs if you want to pull from SDK directly
# (only if you need them and they aren't auto-pulled by prj.conf defaults)
# mcux_add_source(
#   BASE_PATH ${SdkRootDirPath}
#   SOURCES drivers/common/*.c drivers/clock/*.c drivers/iomuxc/*.c drivers/igpio/*.c drivers/lpuart/*.c
# )

# Produce .bin alongside the .elf
mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
