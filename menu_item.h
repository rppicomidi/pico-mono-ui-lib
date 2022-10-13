/**
 * @file menu_item.h
 * @brief this class describes the interface for a single item in a menu
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
#include "view.h"
#include "mono_graphics_lib.h"
namespace rppicomidi {
class Menu_item
{
public:
    Menu_item() = delete;
    Menu_item(const char* text_, Mono_graphics& screen_, const Mono_mono_font& font_) : screen{screen_}, font{font_},
        highlighted{false}, hidden{false}, disabled{false}, last_draw_y{-1} { strncpy(text, text_, max_text_len); text[max_text_len] = '\0'; }
    virtual ~Menu_item() = default;

    /**
     * @brief draw the view to the screen
     * 
     * @param y_ the number of pixels from the top of the screen for the upper left
     * corner of the Menu_item text bounding box
     */
    virtual void draw(uint8_t y_)
    {
        last_draw_y = y_;
        redraw();
    }

    /**
     * @brief update the menu item at the last draw(uint8_t y_) y_ position
     * 
     */
    virtual void redraw()
    {
        if (last_draw_y < 0)
            return;
        if (is_hidden())
            return;
        if (is_highlighted())
            screen.draw_string(font, 0, last_draw_y, text, strlen(text), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
        else
            screen.draw_string(font, 0, last_draw_y, text, strlen(text), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
    }

    /**
     * @brief Menu calls this before it calls draw()
     *
     */
    virtual void entry() { }

    /**
     * @brief menu calls this function for each menu
     * item on Menu::entry() and Menu::exit() and for every menu item that scrolls out of view.
     */
    virtual void exit() {last_draw_y=-1; }

    /**
     * @brief screen manager calls this function in response to a UI
     * action that is supposed to select something.
     *
     * For example, this could be a button press, a touch screen touch,
     * a message over a serial port, etc.
     *
     * @return how focus and current view should change when this function returns.
     */
    virtual View::Select_result on_select(View** new_view_) { *new_view_ = nullptr; return View::Select_result::no_op; }

    /**
     * @brief screen manager calls this function in response to a UI
     * action that is supposed to increment something.
     *
     * For example, this could be a "+" button press, a clocksise twist
     * of a rotary encoder, a mouse wheel spin, etc.
     *
     * @param delta the number of increments
     */
    virtual void on_increment(uint32_t delta, bool is_shifted) { (void)delta; (void)is_shifted;}

    /**
     * @brief screen manager calls this function in response to a UI
     * action that is supposed to increment something.
     *
     * For example, this could be a "-"" button press, a counter-clocksise
     * twist of a rotary encoder, a mouse wheel spin, etc.
     *
     * @param delta the number of decrements; note this is a positive number
     */
    virtual void on_decrement(uint32_t delta, bool is_shifted) {(void)delta; (void)is_shifted;}

    virtual void on_left(uint32_t delta, bool is_shifted) {(void)delta; (void)is_shifted;}
    virtual void on_right(uint32_t delta, bool is_shifted) {(void)delta; (void)is_shifted;}
    virtual void set_highlighted(bool highlighted_) {highlighted = highlighted_; }
    virtual bool is_highlighted() {return highlighted; }
    virtual void set_hidden(bool hidden_) {hidden = hidden_; }
    virtual bool is_hidden()  {return hidden; }
    virtual void set_disabled(bool disabled_) {disabled = disabled_; }
    virtual bool is_disabled()  {return disabled; }
    virtual const char* get_text() {return text; }
    virtual void set_text(const char* new_text) {strncpy(text, new_text, max_text_len); text[max_text_len] = '\0'; }
protected:
    Mono_graphics& screen;
    const Mono_mono_font& font;
    bool highlighted;
    bool hidden;
    bool disabled;
    static const uint8_t max_text_len = 21;
    char text[max_text_len+1];
    int8_t last_draw_y;  // if < 0, never drawn before.
};
}
