# T-Modem-c1224 Pico firmware — ONE UF2 · GPLv3 · AS-IS
# USB KISS half-TNC (HDLC on Pico + bitbang GP6/GP7). No softmodem.
cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED ENV{PICO_SDK_PATH} AND NOT DEFINED PICO_SDK_PATH)
  message(FATAL_ERROR "Set PICO_SDK_PATH to the Raspberry Pi Pico SDK root")
endif()
if(DEFINED ENV{PICO_SDK_PATH} AND NOT DEFINED PICO_SDK_PATH)
  set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(c1224_firmware C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Physical rate = stuffed crystal. Bitbang must match: 1200 (Y1) or 2400 (Y2).
if(NOT DEFINED C1224_BITRATE)
  set(C1224_BITRATE 1200)
endif()
if(NOT C1224_BITRATE EQUAL 1200 AND NOT C1224_BITRATE EQUAL 2400)
  message(FATAL_ERROR "C1224_BITRATE must be 1200 (Y1) or 2400 (Y2)")
endif()

add_executable(c1224
  src/common/main.c
  src/common/status.c
  src/common/ptt.c
  src/common/modem_probe.c
  src/common/usb_cmd.c
  src/common/display.c
  src/common/board_pins.c
  src/common/crystal_steer.c
  src/common/baud_config.c
  src/common/config.c
  src/common/sense.c
  src/common/hdlc.c
  src/common/kiss.c
  src/common/afsk_bitbang.c
  src/common/banner.c
)

target_include_directories(c1224 PRIVATE include)
target_compile_definitions(c1224 PRIVATE C1224_BITRATE=${C1224_BITRATE})
target_link_libraries(c1224
  pico_stdlib
  hardware_gpio
  hardware_i2c
  hardware_flash
  hardware_sync
)

pico_enable_stdio_usb(c1224 1)
pico_enable_stdio_uart(c1224 0)
pico_add_extra_outputs(c1224)

pico_set_binary_type(c1224 default)
