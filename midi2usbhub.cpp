/**
 * @file Pico-USB-Host-MIDI-Adapter.c
 * @brief A USB Host to Serial Port MIDI adapter that runs on a Raspberry Pi Pico board
 *
 * MIT License
 * Copyright (c) 2022 rppicomidi
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

void rppicomidi::Midi2usbhub::serialize(std::string &serialized_string)
{
    serialized_string.clear();
    serialized_string.append("{\"nm\":\"");
    serialized_string.append(name);
    serialized_string.append("\",\"cfg\":");
    if (config)
        config->serialize(serialized_string);
    else
        serialized_string.append("null");
    serialized_string.append(",\"con\":");
    if (connections)
        connections->serialize(serialized_string);
    else
        serialized_string.append("null");
    serialized_string.append("}");
}

void rppicomidi::Midi2usbhub::deserialize(const char *json_str)
{
    jsmn_parser parser;
    jsmn_init(&parser);
    size_t json_len = strlen(json_str);
    int num_tok = jsmn_parse(&parser, json_str, json_len, NULL, 0);
    if (num_tok <= 0)
        return;
    jsmntok_t *tokens = new jsmntok_t[num_tok];
    if (tokens == nullptr)
        return;
    jsmn_init(&parser);
    if (jsmn_parse(&parser, json_str, json_len, tokens, num_tok) != num_tok) {
        delete[] tokens;
        return;
    }
    if (tokens[0].type != JSMN_OBJECT) {
        delete[] tokens;
        return;
    }
    bool have_name = false;
    bool have_config = false;
    bool have_connections = false;
    int name_start = 0;
    int name_len = 0;
    int config_start = 0;
    int config_len = 0;
    int connections_start = 0;
    int connections_len = 0;

    for (int idx = 1; idx < num_tok - 1 && !(have_name && have_config && have_connections); idx += 2) {
        if (tokens[idx].type == JSMN_STRING && tokens[idx + 1].start >= 0) {
            int key_start = tokens[idx].start;
            int key_end = tokens[idx].end;
            if (strncmp(&json_str[key_start], "nm", key_end - key_start) == 0) {
                if (tokens[idx + 1].type == JSMN_STRING) {
                    name_start = tokens[idx + 1].start;
                    name_len = tokens[idx + 1].end - name_start;
                    have_name = true;
                }
            }
            else if (strncmp(&json_str[key_start], "cfg", key_end - key_start) == 0) {
                config_start = tokens[idx + 1].start;
                config_len = tokens[idx + 1].end - config_start;
                have_config = true;
            }
            else if (strncmp(&json_str[key_start], "con", key_end - key_start) == 0) {
                connections_start = tokens[idx + 1].start;
                connections_len = tokens[idx + 1].end - connections_start;
                have_connections = true;
            }
        }
    }
    if (have_name) {
        name.assign(&json_str[name_start], name_len);
    }
    if (have_config && config && config_len > 4) {
        char *config_str = new char[config_len + 1];
        if (config_str) {
            strncpy(config_str, &json_str[config_start], config_len);
            config_str[config_len] = '\0';
            config->deserialize(config_str);
            delete[] config_str;
        }
    }
    if (have_connections && connections && connections_len > 4) {
        char *connections_str = new char[connections_len + 1];
        if (connections_str) {
            strncpy(connections_str, &json_str[connections_start], connections_len);
            connections_str[connections_len] = '\0';
            connections->deserialize(connections_str);
            delete[] connections_str;
        }
    }
    delete[] tokens;
}

int main()
{
    rppicomidi::Midi2usbhub& instance = rppicomidi::Midi2usbhub::instance();
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

void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets)
{
    if (num_packets != 0) {
        rppicomidi::Midi2usbhub::instance().midi_rx_callback(dev_addr, num_packets);
    }
}

void tuh_midi_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
    rppicomidi::Midi2usbhub::instance().midi_mount_callback(dev_addr, instance, desc_report, desc_len);
}

void tuh_midi_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    rppicomidi::Midi2usbhub::instance().midi_umount_callback(dev_addr, instance);
}
