/**
 * @file hid_keyboard.h
 * @brief this class describes a driver for a generic HID keyboard
 * that passes key press events to the View_manager class
 *
 * @note you must define CFG_TUH_HID in tusb_config.h to 3 or 4
 * or else HID reports will not be processed
 *
 * MIT License
 *
 * Copyright (c) 2023 rppicomidi
 *
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
 *
 * Uses substantial code from tinyusb/examples/host/cdc_msc_hid/src/hid_app.c,
 * which is
 * Copyright (c) 2021, Ha Thach (tinyusb.org), same MIT License
 */
#pragma once

#include "view_manager.h"
#include "tusb.h"

namespace rppicomidi
{
class Hid_keyboard
{
public:
    // Singleton Pattern

    /**
     * @brief Get the Instance object
     *
     * @return the singleton instance
     * 
     * @note recommended initial Hid_keyboard object creation:
     * rppicomidi::Hid_keyboard::instance().set_view_manager(pointer_to_the_application_view_manager);
     */
    static Hid_keyboard &instance()
    {
        static Hid_keyboard _instance; // Guaranteed to be destroyed.
                                    // Instantiated on first use.
        return _instance;
    }
    Hid_keyboard(Hid_keyboard const &) = delete;
    void operator=(Hid_keyboard const &) = delete;
    ~Hid_keyboard()=default;
    /**
     * @brief Set the view manager so this code knows where to report keyboard events
     * 
     * @param vm_ a pointer to the View_manager for the application
     */
    void set_view_manager(View_manager* vm_) {vm = vm_; }

    /**
     * @brief translate the HID keyboard code and modifier byte into an ASCII character if possible
     * 
     * @param code the HID keyboard code
     * @param modifiers the HID modifier byte
     * @return the translated character or '\0' if the character can not be translated
     */
    static char translate_keycode(uint8_t code, uint8_t modifiers);

    /**
     * @brief tinyusb HID mount callback implementation; needs to be public, but do not call directly
     */
    void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len);

    /**
     * @brief tinyusb HID report received callback implementation; needs to be public, but do not call directly
     */
    void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len);
private:
    Hid_keyboard() : vm{nullptr} {memset(&prev_report, 0, sizeof(prev_report)); }
    void process_kbd_report(hid_keyboard_report_t const *report);
    void process_generic_report(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len);
    // look up new key in previous keys
    static inline bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t keycode) {
        for(uint8_t idx=0; idx<6; idx++) {
            if (report->keycode[idx] == keycode)
                return true;
        }
        return false;
    }
    View_manager* vm;
    static uint8_t constexpr keycode2ascii[128][2] =  { HID_KEYCODE_TO_ASCII };

    static size_t const MAX_REPORT = 4;

    // Each HID instance can has multiple reports
    struct {
        uint8_t report_count;
        tuh_hid_report_info_t report_info[MAX_REPORT];
    } hid_info[CFG_TUH_HID];
    hid_keyboard_report_t prev_report;
};
} // namespace rppicomidi
