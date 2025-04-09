/**
 * @file Pico-USB-Host-MIDI-Adapter.c
 * @brief A USB Host to Serial Port MIDI adapter that runs on a Raspberry Pi Pico board
 *
 * MIT License
 * Copyright (c) 2022 rppicomidi
 * [License text omitted for brevity]
 */

#include <cstdio>
#include <vector>
#include <cstdint>
#include <string>
#include "midi2usbhub.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "midi_uart_lib.h"
#include "bsp/board_api.h"
#include "preset_manager.h"
#include "diskio.h"
#ifdef RPPICOMIDI_PICO_W
#include "pico/cyw43_arch.h"
#endif
#include "tusb.h"  // Bereits vorhanden, für TinyUSB

// USB String-Deskriptoren
// char const* string_desc_arr[] = {
//     (const char[]) { 0x09, 0x04 },   // 0: LangID = English (0x0409)
//    "My Company",                    // 1: Manufacturer
//    "RPI-PICO-Name-Name",            // 2: Product
//    "123456",                        // 3: Serial Number
// };

// TinyUSB Callback für String-Deskriptoren (Device Mode)
// uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
//     static uint16_t desc_str[32];
//     uint8_t chr_count;

 //    if (index == 0) {
 //        desc_str[1] = (TUSB_DESC_STRING << 8) | (2 * 1 + 2);
//         desc_str[2] = string_desc_arr[0][0] | (string_desc_arr[0][1] << 8);
 //        return desc_str;
 //    }

//     if (index >= sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) {
 //        return NULL;
 //    }

//     const char* str = string_desc_arr[index];
//     chr_count = strlen(str);
//     if (chr_count > 31) chr_count = 31;  // Begrenze auf maximale Länge

//    desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
//     for (uint8_t i = 0; i < chr_count; i++) {
//        desc_str[1 + i] = str[i];
//     }
// 
//    return desc_str;
// }

// Rest der Datei bleibt unverändert bis hierher
void rppicomidi::Midi2usbhub::serialize(std::string &serialized_string)
{
    // [Unveränderter Code]
}

// [Weitere Funktionen unverändert bis main()]

int main()
{
    rppicomidi::Midi2usbhub &instance = rppicomidi::Midi2usbhub::instance();
#ifdef RPPICOMIDI_PICO_W
    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return -1;
    }
#endif
    while (1) {
        instance.task();
    }
}

// [Rest der Datei mit TinyUSB Callbacks unverändert]
