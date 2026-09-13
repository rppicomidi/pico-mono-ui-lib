/**
 * @file nav_buttons.h
 * @brief this class describes a driver for 7 nav buttons or a 5-way
 * "joystick" switch plus two nav buttons. It triggers View_manager
 * events on button presses.
 *
 * The button mapping to GPIO pins on the RP2040 is defined using
 * the BUTTON_* macros below by default unless they are defined
 * in the CMakeLists.txt file
 *
 * MIT License
 *
 * Copyright (c) 2022 rppicomidi
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
 */
#pragma once
#include "view_manager.h"
// Button GP number default definitions
#ifndef BUTTON_UP
#define BUTTON_UP 11
#endif
#ifndef BUTTON_DOWN
#define BUTTON_DOWN 8
#endif
#ifndef BUTTON_LEFT
#define BUTTON_LEFT 10
#endif
#ifndef BUTTON_RIGHT
#define BUTTON_RIGHT 6
#endif
#ifndef BUTTON_ENTER
#define BUTTON_ENTER 9
#endif
#ifndef BUTTON_BACK
#define BUTTON_BACK 12
#endif
#ifndef BUTTON_SHIFT
#define BUTTON_SHIFT 7
#endif

/**
 * Set NAV_BUTTONS_3_BUTTON_MODE to 1 to use only the UP, DOWN, and ENTER
 * buttons. In this mode the following button chords provide the missing
 * navigation actions:
 *
 *   UP + DOWN          -> Back
 *   ENTER + UP         -> Right
 *   ENTER + DOWN       -> Shift + Left
 *   ENTER + UP + DOWN  -> Home
 *
 * Chord actions and ENTER are generated once per press. UP and DOWN retain
 * key-repeat behavior when pressed individually.
 */
#ifndef NAV_BUTTONS_3_BUTTON_MODE
#define NAV_BUTTONS_3_BUTTON_MODE 0
#endif

namespace rppicomidi {
class Nav_buttons
{
public:
    Nav_buttons(View_manager& view_manager_);
    void poll();
private:
    const char* get_button_name(uint32_t button_map);
    View_manager& view_manager;
    static const uint8_t ndebounce=10;
    uint32_t debounce[ndebounce];
    uint32_t prev_buttons;
    absolute_time_t previous_timestamp;
    int32_t held_buttons_timeout;
    const int32_t max_button_repeat_interval_ms;
    int32_t button_repeat_interval_ms;
    int32_t acceleration_count;
    enum Btn_idx {BTN_IDX_UP, BTN_IDX_DN, BTN_IDX_LF, BTN_IDX_RT, BTN_IDX_EN, BTN_IDX_BK, BTN_IDX_SH};
    static const uint8_t nbuttons = 7;
    uint32_t button_mask[nbuttons]; // button_mask[BTN_IDX_SH] is the bit mask of shift button
};
}