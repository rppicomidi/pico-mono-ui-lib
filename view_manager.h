/**
 * @file view_manager.h
 * @brief this class manages what view is currently visible and dispatches
 * UI events like rotary encoder twists, button presses, etc. to the
 * currently visible view.
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
#include <vector>
#include "view.h"
namespace rppicomidi {
class View_manager
{
public:
    View_manager() { current_view = view_stack.end(); }

    /**
     * @brief make the new view the current view
     * 
     * @param new_view the new view object at the top of the view stack
     */
    void push_view(View* new_view);

    /**
     * @brief pop the current view off of the view stack and make
     * the next view down the current view
     */
    void pop_view();

    /**
     * @brief pop all views off the stack except the first view, which
     * should be the home screen
     */
    void go_home();

    /**
     * @brief Get the current view object
     * 
     * @return a reference to the item on the top of the view stack
     */
    View* get_current_view() {return *current_view; }

    /**
     * @brief check if a view is the current view
     *
     * @param test 
     * @return true if test points to the current view, false otherwise
     */
    bool is_current_view(View* test) {return test == (*current_view);}

    /**
     * @brief convey to the current view that the UI increment action has occurred
     * 
     * @param delta 
     */
    void on_increment(uint32_t delta, bool is_shifted) {if (current_view != view_stack.end()) (*current_view)->on_increment(delta, is_shifted); }

    /**
     * @brief convey to the current view that the UI decrement action has occurred
     * 
     * @param delta 
     */
    void on_decrement(uint32_t delta, bool is_shifted) {if (current_view != view_stack.end()) (*current_view)->on_decrement(delta, is_shifted); }

    /**
     * @brief convey to the current view that the UI left (horizontal decrement) action has occurred
     * 
     * @param delta 
     */
    void on_left(uint32_t delta, bool is_shifted) {if (current_view != view_stack.end()) (*current_view)->on_left(delta, is_shifted); }

    /**
     * @brief convey to the current view that the UI right (horizontal increment) action has occurred
     * 
     * @param delta 
     */
    void on_right(uint32_t delta, bool is_shifted) {if (current_view != view_stack.end()) (*current_view)->on_right(delta, is_shifted); }

    /**
     * @brief convey to the current view that the UI select action has occurred
     */
    void on_select();

    /**
     * @brief convey to the current view that the UI keyboard action has occurred
     *
     * @param key_code is the code for the keyboard key
     * @param modifiers is the modifier bits for the keyboard report
     * @param pressed is true if the key was pressed and false if it was released
     */
    void on_key(uint8_t key_code, uint8_t modifiers, bool pressed) {if (current_view != view_stack.end()) (*current_view)->on_key(key_code, modifiers, pressed); }
private:
    //void switch_current_view();
    std::vector<View*> view_stack;
    std::vector<View*>::iterator current_view;
};
}
