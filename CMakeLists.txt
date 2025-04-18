cmake_minimum_required(VERSION 3.13)

set(BOARD pico_sdk)
# Set the TinyUSB log level
# set(LOG 2)
include(pico_sdk_import.cmake)

project(midi2usbhub C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPICO_USE_MALLOC_MUTEX=1")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ext_lib/littlefs-lib ext_lib/littlefs-lib)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ext_lib/fatfs/source)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/midi_uart_lib)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/rp2040_rtc)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/usb_midi_host)
set(EMBEDDED_CLI_PATH ${CMAKE_CURRENT_LIST_DIR}/ext_lib/embedded-cli/lib/)
add_executable(midi2usbhub
    midi2usbhub.cpp
    preset_manager.cpp
    pico_lfs_cli.cpp
    pico_fatfs_cli.cpp
    preset_manager_cli.cpp
    midi2usbhub_cli.cpp
    ${EMBEDDED_CLI_PATH}/src/embedded_cli.c
    ${CMAKE_CURRENT_LIST_DIR}/ext_lib/parson/parson.c
)

pico_enable_stdio_uart(midi2usbhub 1)

target_include_directories(midi2usbhub PRIVATE
 ${CMAKE_CURRENT_LIST_DIR}
 ${EMBEDDED_CLI_PATH}/include
 ${CMAKE_CURRENT_LIST_DIR}/ext_lib/parson
)

target_link_options(midi2usbhub PRIVATE -Xlinker --print-memory-usage)
target_compile_options(midi2usbhub PRIVATE -Wall -Wextra -DPICO_HEAP_SIZE=0x20000)
target_link_libraries(midi2usbhub midi_uart_lib tinyusb_host tinyusb_board usb_midi_host_app_driver ring_buffer_lib pico_stdlib
littlefs-lib msc_fatfs rp2040_rtc)
target_link_options(midi2usbhub PRIVATE -Xlinker --print-memory-usage)
if(DEFINED PICO_BOARD)
if(${PICO_BOARD} MATCHES "pico_w")
message("board is pico_w")
# add additional compile and link options
target_compile_options(midi2usbhub PRIVATE -DRPPICOMIDI_PICO_W)
target_link_libraries(midi2usbhub pico_cyw43_arch_none)
set(RPPICOMIDI_PICO_W 1)
else()
message("board is pico")
endif()
else()
message("board is not defined")
endif()

pico_add_extra_outputs(midi2usbhub)
