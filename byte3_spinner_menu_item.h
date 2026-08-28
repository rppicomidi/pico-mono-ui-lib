/**
 * @file byte3_spinner_menu_item.h
 * @author rppicomidi
 * @brief This template class allows you to edit three byte
 * values intended for Setting_byte3 objects.
 * 
 * Format of the menu item is 
 * "text" "byte0" "byte1" "byte2"
 * Examples:
 *  In:90 44 0
 * Out:90 54 1
 *
 * MIT License
 *
 * Copyright (c) 2026 rppicomidi
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
#include <cstdio>
#include <cstdint>
#include "view.h"
#include "menu_item.h"
#include "setting_number.h"
#include "mono_graphics_lib.h"
namespace rppicomidi
{
class Byte3_spinner_menu_item : public Menu_item
{
public:
    Byte3_spinner_menu_item()=delete;
    /**
     * @brief Construct a new Byte3_spinner_menu_item object
     * 
     * @param text_ The text label for the int spinner value
     * @param screen_ The screen object that renders this menu item
     * @param font_  The font to render the label and value
     * @param byte3_idx_ The index in the Setting_byte3 object to edit
     * @param hex_format_ True to render the int value in hex format; false to render in base 10
     * @param get_fn_ The function to get the setting value
     * @param incr_fn_ The function to increment (or decrement, if the int delta is negative) the currently selected setting value
     * @param context_ A pointer to the class that contains the static get_fn and incr_fn cast to void*
     */
    Byte3_spinner_menu_item(const char* text_, Mono_graphics& screen_, const Mono_mono_font& font_,
        size_t byte3_idx_, bool hex_format_,
        uint8_t (*get_fn_)(void* context_, size_t byte3_idx), uint8_t (*get_max_fn_)(void* context_, size_t byte3_idx),
        uint8_t (*incr_fn_)(void* context_, size_t byte3_idx, int delta), void* context_) :
        Menu_item{text_, screen_, font_},
        byte3_idx{byte3_idx_},
        ndigits{3}, nhex_digits{2}, hex_format{hex_format_},
        get_fn{get_fn_}, get_max_fn{get_max_fn_}, incr_fn{incr_fn_}, context{context_}
    {
        editing = 3;
    }

    virtual void redraw()
    {
        if (is_hidden())
            return;
        uint8_t first = get_fn(context, 0);
        uint8_t second = get_fn(context, 1);
        uint8_t third = get_fn(context, 2);
        char first_str[ndigits+1];
        char second_str[ndigits+1];
        char third_str[ndigits+1];
        if (hex_format) {
            snprintf(first_str, nhex_digits+1, "%0*X", nhex_digits, first);
            snprintf(second_str, nhex_digits+1, "%0*X", nhex_digits, second);
            snprintf(third_str, nhex_digits+1, "%0*X", nhex_digits, third);
        }
        else {
            snprintf(first_str, ndigits+1, "%*d", ndigits, first);
            snprintf(second_str, ndigits+1, "%*d", ndigits, second);
            snprintf(third_str, ndigits+1, "%*d", ndigits, third);
        }

        if (is_highlighted() && editing == 3) {
            // draw the label and all 3 numbers in reverse text
            screen.draw_string(font, 0, last_draw_y, text, strlen(text), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            screen.draw_string(font, strlen(text)*font.width, last_draw_y, first_str, strlen(first_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            screen.draw_string(font, (strlen(text)+strlen(first_str))*font.width, last_draw_y, " ", 1, Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            screen.draw_string(font, (strlen(text)+strlen(first_str)+1)*font.width, last_draw_y, second_str, strlen(second_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            screen.draw_string(font, (strlen(text)+strlen(first_str)+strlen(second_str)+1)*font.width, last_draw_y, " ", 1, Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            screen.draw_string(font, (strlen(text)+strlen(first_str)+strlen(second_str)+2)*font.width, last_draw_y, third_str, strlen(third_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
        }
        else {
            // The label text and the all the digits are normal text except the currently editing number is in reverse text
            screen.draw_string(font, 0, last_draw_y, text, strlen(text), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            screen.draw_string(font, (strlen(text)+strlen(first_str))*font.width, last_draw_y, " ", 1, Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            screen.draw_string(font, (strlen(text)+strlen(first_str)+strlen(second_str)+1)*font.width, last_draw_y, " ", 1, Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            if (editing == 0) {
                screen.draw_string(font, strlen(text)*font.width, last_draw_y, first_str, strlen(first_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+1)*font.width, last_draw_y, second_str, strlen(second_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+strlen(second_str)+2)*font.width, last_draw_y, third_str, strlen(third_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            }
            else if (editing == 1) {
                screen.draw_string(font, strlen(text)*font.width, last_draw_y, first_str, strlen(first_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+1)*font.width, last_draw_y, second_str, strlen(second_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+strlen(second_str)+2)*font.width, last_draw_y, third_str, strlen(third_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            }
            else if (editing == 2) {
                screen.draw_string(font, strlen(text)*font.width, last_draw_y, first_str, strlen(first_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+1)*font.width, last_draw_y, second_str, strlen(second_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+strlen(second_str)+2)*font.width, last_draw_y, third_str, strlen(third_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            }
            else {
                screen.draw_string(font, strlen(text)*font.width, last_draw_y, first_str, strlen(first_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+1)*font.width, last_draw_y, second_str, strlen(second_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+strlen(second_str)+2)*font.width, last_draw_y, third_str, strlen(third_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            }
        }
    }

    void entry() final
    {
        editing = 3;
    }

    void exit() final
    {
        editing = 3;
        last_draw_y=-1;
    }

    void on_increment(uint32_t delta, bool is_shifted) final
    {
        if (is_shifted)
            delta *= 10;

        incr((int)delta);
    }

    void on_decrement(uint32_t delta, bool is_shifted) final
    {
        if (is_shifted)
            delta *= 10;

        incr(-(int)delta);
    }

    virtual void on_left(uint32_t, bool)
    {
        if (editing == 2)
            editing = 1;
        else if (editing == 1)
            editing = 0;
        redraw();
    }

    virtual void on_right(uint32_t, bool)
    {
        if (editing == 0)
            editing = 1;
        else if (editing == 1)
            editing = 2;
        redraw();
    }

    virtual View::Select_result on_select(View**)
    {
        if (editing != 3) {
            editing = 3;
        }
        else {
            editing = 0;
        }
        redraw();
        if (editing != 3)
            return View::Select_result::take_focus;
        return View::Select_result::give_focus;
    }

    virtual size_t get_byte3_idx() const {
        return byte3_idx;
    }

    void set_display_hex(bool is_hex) {
        hex_format = is_hex;
    }
protected:
    void incr(int delta)
    {
        if (editing == 3)
            return;
        uint8_t oldval = get_fn(context, editing);
        uint8_t newval = incr_fn(context, editing, delta);
        if (oldval != newval)
            redraw();
    }
    size_t byte3_idx;
    int ndigits;
    int nhex_digits;
    bool hex_format;
    uint8_t (*get_fn)(void* context_, size_t byte3_idx);
    uint8_t (*get_max_fn)(void* context_, size_t byte3_idx);
    uint8_t (*incr_fn)(void* context_, size_t byte3_idx, int delta);
    void* context;
    size_t editing;       //<! 0 if editing the first value, 1 if editing the second value, 2 if editing the third value, 3 if not editing
};
}
