cmake_minimum_required(VERSION 3.13)
set(BOARD pico_sdk)
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
add_executable(midi2usbhub
    midi2usbhub.cpp
    midi2usbhub_cli.cpp
    preset_manager.cpp
)
pico_enable_stdio_uart(midi2usbhub 1)
target_include_directories(midi2usbhub PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(midi2usbhub midi_uart_lib tinyusb_host tinyusb_board usb_midi_host_app_driver ring_buffer_lib pico_stdlib littlefs-lib msc_fatfs rp2040_rtc)
pico_add_extra_outputs(midi2usbhub)
