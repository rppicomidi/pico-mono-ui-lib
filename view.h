/**
 * @file ui_view.h
 * @brief this class describes the interface for the MVC view
 * for a Raspberry Pi Pico UI framework.
 *
 * The only mandatory method in this class is draw(). This class assumes
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

#include "mono_graphics_lib.h"
namespace rppicomidi {
class View
{
public:
    enum Select_result {
        no_op,      //!< nothing happens as a result of select
        new_view,   //!< a new view has been pushed to the view stack
        take_focus, //!< the current view is taking focus
        give_focus, //!< the current view is relinquishing focus
        exit_view,  //!< the current view is done and should be popped from the stack
    };
    /**
     * @brief Construct a new View object
     * 
     * @param rect the bounding rectangle of the view
     */
    View(Mono_graphics& screen_, const Rectangle& rect_) :
        screen{screen_}, view_rect{rect_}, has_focus{false} {}
    virtual ~View() = default;

    /**
     * @brief draw the view to the screen
     */
    virtual void draw()=0;

    /**
     * @brief screen manager calls this function prior to calling draw()
     *
     * Typical actions would be to load settings from the model, subscribe
     * to changes in settings from the model, etc.
     */
    virtual void entry() {}

    /**
     * @brief screen manager calls this function before calling entry()
     * for the next screen
     *
     * Typical actions would be to unsubscribe to changes in model settings
     */
    virtual void exit() {}

    /**
     * @brief screen manager calls this function in response to a UI
     * action that is supposed to select something.
     *
     * For example, this could be a button press, a touch screen touch,
     * a message over a serial port, etc.
     * 
     * @param new_view_ a pointer to storage to set to the pointer to the
     * new view if the return value is new_view, otherwise set to a nullptr
     *
     * @return how focus and current view should change when this function returns.
     */
    virtual Select_result on_select(View** new_view_) { *new_view_ = nullptr; return Select_result::no_op; }

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
    /**
     * @brief screen manager calls this function in response to a real
     * (or virtual) button press or release
     *
     * @param key_code a number to identify the button: application dependent
     * @param pressed true if the key is pressed, false if released
     */
    virtual void on_key(uint32_t key_code, bool pressed) {(void)key_code; (void)pressed;}

    virtual Select_result set_has_focus(bool has_focus_) {
        has_focus = has_focus_;
        if (has_focus)
            return Select_result::take_focus;
        return Select_result::give_focus;
    }

    virtual bool get_has_focus() {return has_focus; }
protected:
    Mono_graphics& screen;
    Rectangle view_rect;
    bool has_focus;
};
}