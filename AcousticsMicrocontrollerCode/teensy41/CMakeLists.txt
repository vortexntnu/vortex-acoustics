cmake_minimum_required(VERSION 3.22)

# Avoid hosted link during try-compile on bare metal
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# find_package(Mcuxsdk REQUIRED)

# --- EDIT: your SDK "mcuxsdk" root (has cmake/extension/mcux.cmake) ---
set(SdkRootDirPath "$ENV{HOME}/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk" CACHE PATH "")

# Pull in NXP CMake extensions (defines mcux_add_source/mcux_add_include/etc.)
include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

set(CONFIG_TOOLCHAIN "armgcc" CACHE STRING "" FORCE)
set(CONFIG_MCUX_PRJSEG_module.board OFF CACHE BOOL "" FORCE)
set(CONFIG_MCUX_PRJSEG_module.board.pinmux_project_folder OFF CACHE BOOL "" FORCE)
set(CONFIG_MCUX_COMPONENT_CMSIS_DSP_Include TRUE CACHE BOOL "" FORCE)
set(CONFIG_MCUX_COMPONENT_CMSIS_DSP_Source TRUE CACHE BOOL "" FORCE)
# set(CONFIG_MCUX_COMPONENT_utility.debug_console TRUE CACHE BOOL "" FORCE) 
# set(CONFIG_MCUX_COMPONENT_utility.debug_console_template_config TRUE CACHE BOOL "" FORCE)


set(CONFIG_MCUX_COMPONENT_utility.debug_console_lite TRUE CACHE BOOL "" FORCE)

set(CONFIG_MCUX_COMPONENT_component.serial_manager TRUE CACHE BOOL "" FORCE)
set(CONFIG_MCUX_COMPONENT_component.serial_manager TRUE CACHE BOOL "" FORCE)
set(CONFIG_MCUX_COMPONENT_component.serial_manager_usb_cdc TRUE CACHE BOOL "" FORCE)
set(CONFIG_MCUX_COMPONENT_component.lists TRUE CACHE BOOL "" FORCE) 

set(CONFIG_MCUX_COMPONENT_component.osa_template_config TRUE CACHE BOOL "" FORCE)
set(CONFIG_MCUX_COMPONENT_component.osa_bm TRUE CACHE BOOL "" FORCE)
set(CONFIG_MCUX_COMPONENT_middleware.usb.common_header TRUE CACHE BOOL "" FORCE)
set(CONFIG_MCUX_COMPONENT_middleware.usb.device.common_header TRUE CACHE BOOL "" FORCE)

set(CONFIG_MCUX_COMPONENT_middleware.usb.device.cdc TRUE CACHE BOOL "" FORCE)

set(CONFIG_MCUX_COMPONENT_middleware.usb.phy TRUE CACHE BOOL "" FORCE)

# Device-only project (no board path)
project(teensy41 LANGUAGES C ASM)

# Tell the SDK which device/core/toolchain you're targeting.
# (We pass them on the cmake command line too; setting here helps for IDEs.)
set(device  "MIMXRT1062"   CACHE STRING "" FORCE)
set(core_id "cm7"           CACHE STRING "" FORCE)
# Bring in the SDK’s top-level CMake so device/drivers can be found

# ---- Your sources / includes (extension-style) ----
# Use your own project root as BASE_PATH for convenience

mcux_add_cmakelists(${SdkRootDirPath}/${device_root}/${soc_portfolio}/${soc_series}/${device})
mcux_add_cmakelists(${SdkRootDirPath}/components)
mcux_add_cmakelists(${SdkRootDirPath}/arch/arm/CMSIS)
mcux_add_cmakelists(${SdkRootDirPath}/middleware/usb)
# mcux_add_cmakelists(${SdkRootDirPath}/components/debug_console)
# mcux_add_cmakelists(${SdkRootDirPath}/components/lists)
# mcux_add_cmakelists(${SdkRootDirPath}/components/str)
# mcux_add_cmakelists(${SdkRootDirPath}/components/serial_manager)
mcux_load_all_cmakelists_in_directory(${SdkRootDirPath}/drivers)

mcux_add_include(
  BASE_PATH ${CMAKE_SOURCE_DIR}
  INCLUDES include
           board
)

mcux_add_source(
  BASE_PATH ${CMAKE_SOURCE_DIR}
  SOURCES src/main.c
          src/dsp.c
          src/adc.c
          src/pit.c
          src/clock.c
          src/multilateration.c
          board/board.c
          board/clock_config.c
          board/dcd.c
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
