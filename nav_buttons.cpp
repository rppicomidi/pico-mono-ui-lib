/**
 * @file nav_buttons.cpp
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
#include <cstdio>
#include <cstdint>
#include <cstring>
#include "pico/stdlib.h"
#include "nav_buttons.h"

namespace {
#if NAV_BUTTONS_3_BUTTON_MODE
constexpr uint32_t button_gpio_mask =
    (1u << BUTTON_UP) |
    (1u << BUTTON_DOWN) |
    (1u << BUTTON_ENTER);
#else
constexpr uint32_t button_gpio_mask =
    (1u << BUTTON_UP) |
    (1u << BUTTON_DOWN) |
    (1u << BUTTON_LEFT) |
    (1u << BUTTON_RIGHT) |
    (1u << BUTTON_ENTER) |
    (1u << BUTTON_BACK) |
    (1u << BUTTON_SHIFT);
#endif

void setup_button_gpio(uint32_t gpio)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_up(gpio);
}
}

rppicomidi::Nav_buttons::Nav_buttons(View_manager& view_manager_) :
    view_manager{view_manager_}, prev_buttons{0}, previous_timestamp{get_absolute_time()},
    held_buttons_timeout{0},max_button_repeat_interval_ms{400}, button_repeat_interval_ms{max_button_repeat_interval_ms},
    acceleration_count{10}
{
    // Set up the button GPIO
    setup_button_gpio(BUTTON_UP);
    setup_button_gpio(BUTTON_DOWN);
    setup_button_gpio(BUTTON_ENTER);
#if !NAV_BUTTONS_3_BUTTON_MODE
    setup_button_gpio(BUTTON_LEFT);
    setup_button_gpio(BUTTON_RIGHT);
    setup_button_gpio(BUTTON_BACK);
    setup_button_gpio(BUTTON_SHIFT);
#endif
    memset(debounce, 0, sizeof(debounce));

    button_mask[BTN_IDX_UP] = 1u << BUTTON_UP;
    button_mask[BTN_IDX_DN] = 1u << BUTTON_DOWN;
    button_mask[BTN_IDX_LF] = 1u << BUTTON_LEFT;
    button_mask[BTN_IDX_RT] = 1u << BUTTON_RIGHT;
    button_mask[BTN_IDX_EN] = 1u << BUTTON_ENTER;
    button_mask[BTN_IDX_BK] = 1u << BUTTON_BACK;
    button_mask[BTN_IDX_SH] = 1u << BUTTON_SHIFT;
}

void rppicomidi::Nav_buttons::poll()
{
    absolute_time_t now = get_absolute_time();

    int64_t diff = absolute_time_diff_us(previous_timestamp, now);
    // poll no more often than once per millisecond
    if (diff < 1000)
        return;
    previous_timestamp = now;

    uint32_t buttons = (~gpio_get_all()) & button_gpio_mask;
    bool still_bouncing = (buttons != debounce[0]);
    for (int idx=1; idx < ndebounce; idx ++) {
        still_bouncing = still_bouncing || (buttons != debounce[idx]);
        debounce[idx-1] = debounce[idx];
    }
    debounce[ndebounce-1] = buttons;

    if (!still_bouncing) {
        if (buttons != prev_buttons || held_buttons_timeout <= 1) {
#if NAV_BUTTONS_3_BUTTON_MODE
            bool is_up = (buttons & button_mask[BTN_IDX_UP]) != 0;
            bool is_down = (buttons & button_mask[BTN_IDX_DN]) != 0;
            bool is_enter = (buttons & button_mask[BTN_IDX_EN]) != 0;
            bool state_changed = buttons != prev_buttons;
            bool repeatable_action = false;

            if (state_changed) {
                // Chords are checked before individual buttons so a chord
                // generates exactly one navigation action.
                if (is_enter && is_up && is_down) {
                    view_manager.go_home();
                }
                else if (is_up && is_down) {
                    view_manager.on_back();
                }
                else if (is_enter && is_down) {
                    view_manager.on_left(1, true);
                }
                else if (is_enter && is_up) {
                    view_manager.on_right(1, false);
                }
                else if (is_enter) {
                    view_manager.on_select();
                }
                else if (is_down) {
                    view_manager.on_decrement(1, false);
                    repeatable_action = true;
                }
                else if (is_up) {
                    view_manager.on_increment(1, false);
                    repeatable_action = true;
                }
            }
            else if (held_buttons_timeout <= 1) {
                // Only single UP/DOWN presses repeat. Repeating ENTER, Back,
                // Home, or shifted-left could trigger multiple selections,
                // view pops, or destructive operations.
                if (is_down && !is_up && !is_enter) {
                    view_manager.on_decrement(1, false);
                    repeatable_action = true;
                }
                else if (is_up && !is_down && !is_enter) {
                    view_manager.on_increment(1, false);
                    repeatable_action = true;
                }
            }

            if (repeatable_action && held_buttons_timeout <= 1) {
                if (--acceleration_count <= 0) {
                    acceleration_count = 10;
                    button_repeat_interval_ms -= 100;
                    if (button_repeat_interval_ms < 100)
                        button_repeat_interval_ms = 100;
                }
            }
#else
            bool is_shifted = (buttons & button_mask[BTN_IDX_SH]) != 0;
            for (uint8_t idx = 0; idx < nbuttons; idx++) {
                if (button_mask[idx] & buttons) {
                    switch (idx) {
                    case BTN_IDX_UP:
                        view_manager.on_increment(1, is_shifted);
                        break;
                    case BTN_IDX_DN:
                        view_manager.on_decrement(1, is_shifted);
                        break;
                    case BTN_IDX_LF:
                        view_manager.on_left(1, is_shifted);
                        break;
                    case BTN_IDX_RT:
                        view_manager.on_right(1, is_shifted);
                        break;
                    case BTN_IDX_BK:
                        if (is_shifted)
                            view_manager.go_home();
                        else
                            view_manager.on_back();
                        break;
                    case BTN_IDX_EN:
                        view_manager.on_select();
                        break;
                    default:
                        break;
                    }
                }
            }
            if (held_buttons_timeout <=1) {
                if (--acceleration_count <=0) {
                    acceleration_count = 10;
                    button_repeat_interval_ms -= 100;
                    if (button_repeat_interval_ms < 100)
                        button_repeat_interval_ms = 100;
                }
            }
#endif
            prev_buttons = buttons;
        }
        if (--held_buttons_timeout <= 0) {
            held_buttons_timeout = button_repeat_interval_ms;
        }
    }
    else {
        button_repeat_interval_ms = max_button_repeat_interval_ms;
        held_buttons_timeout = button_repeat_interval_ms;
        acceleration_count = 10;
    }
}

const char* rppicomidi::Nav_buttons::get_button_name(uint32_t button_map)
{
    for (uint8_t idx = 0; idx < nbuttons; idx++) {
        if (button_mask[idx] & button_map) {
            switch (idx) {
            case BTN_IDX_UP:
                return "UP";
            case BTN_IDX_DN:
                return "DOWN";
            case BTN_IDX_LF:
                return "LEFT";
            case BTN_IDX_RT:
                return "RIGHT";
            case BTN_IDX_SH:
                return "HOME";
            case BTN_IDX_BK:
                return "BACK";
            case BTN_IDX_EN:
                return "ENTER";
            default:
                return "UNKNONW";
            }
        }
    }
    return "NONE PRESSED";
}
