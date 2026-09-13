/**
 * @file text_spinner_menu_item.h
 *
 * This class implements a menu item that lets you edit a text (enum) value.
 * When the user presses the Select button, the text field is highlighted. The
 * Up or Down buttons select the next or previous value. Press the Select button
 * again when edit is done. This class works with any other class that implements
 * a string value get function and a string value increment function.
 *
 * MIT License
 *
 * Copyright (c) 2024 TylorShine
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

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

#include "view.h"
#include "menu_item.h"
#include "mono_graphics_lib.h"

namespace rppicomidi
{
class Text_spinner_menu_item : public Menu_item
{
public:
    Text_spinner_menu_item()=delete;

    /**
     * @brief Construct a new Text_spinner_menu_item object
     *
     * @param text_ The text label for the text spinner value
     * @param screen_ The screen object that renders this menu item
     * @param font_ The font to render the label and value
     * @param ndigits_ The minimum number of characters in the value field
     * @param get_fn_ The function to get the setting value
     * @param incr_fn_ The function to increment (or decrement, if delta is negative) the setting value
     * @param context_ A pointer to the class that contains the static get_fn and incr_fn cast to void*
     * @param units_ Optional units suffix (up to max_units_characters characters)
     */
    Text_spinner_menu_item(const char* text_, Mono_graphics& screen_, const Mono_mono_font& font_,
        int ndigits_,
        const std::string (*get_fn_)(void*), const std::string (*incr_fn_)(void*, int),
        void* context_, const char* units_=nullptr) :
        Menu_item{text_, screen_, font_},
        ndigits{ndigits_}, get_fn{get_fn_}, incr_fn{incr_fn_}, context{context_}
    {
        assert(ndigits > 0);
        assert(get_fn != nullptr);
        assert(incr_fn != nullptr);
        editing = false;
        memset(units, 0, sizeof(units));
        if (units_)
            set_units(units_);
    }

    void redraw() override
    {
        if (is_hidden())
            return;

        std::string value = get_fn(context);
        if ((int)value.length() < ndigits)
            value.append(ndigits - value.length(), ' ');
        value += units;

        if (is_highlighted() && !editing) {
            // Draw both the label and value in reverse text.
            screen.draw_string(font, 0, last_draw_y, text, strlen(text), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            screen.draw_string(font, strlen(text)*font.width, last_draw_y, value.c_str(), value.length(), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
        }
        else {
            screen.draw_string(font, 0, last_draw_y, text, strlen(text), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            if (editing) {
                screen.draw_string(font, strlen(text)*font.width, last_draw_y, value.c_str(), value.length(), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            }
            else {
                screen.draw_string(font, strlen(text)*font.width, last_draw_y, value.c_str(), value.length(), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            }
        }
    }

    void entry() final
    {
        editing = false;
    }

    void exit() final
    {
        editing = false;
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

    View::Select_result on_select(View**) override
    {
        editing = !editing;
        redraw();
        if (editing)
            return View::Select_result::take_focus;
        return View::Select_result::give_focus;
    }

    void set_units(const char* units_)
    {
        strncpy(units, units_, max_units_characters);
        units[max_units_characters]='\0';
    }

protected:
    void incr(int delta)
    {
        if (!editing)
            return;
        std::string oldval = get_fn(context);
        std::string newval = incr_fn(context, delta);
        if (oldval != newval)
            redraw();
    }

    int ndigits;
    const std::string (*get_fn)(void*);
    const std::string (*incr_fn)(void*, int);
    void* context;
    bool editing;       //<! true if editing the value; false if displaying it as text only
    static const uint8_t max_units_characters=4;
    char units[max_units_characters+1];
};
}
