/**
 * @file callback_menu_item.h
 *
 * This file implements a Menu_item that calls a callback function when
 * it is selected. I was too lazy to use std::function and std::bind,
 * so I just use C-style function pointers.
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
#include "menu_item.h"
#include "view.h"
namespace rppicomidi
{
class Callback_menu_item : public Menu_item
{
public:
    /**
     * @brief Construct a new Callback_menu_item object
     *
     * @param text_ the menu item text
     * @param screen_ the drawing screen object
     * @param font_ the font to render text
     * @param callback_view_ the View that implements the callback function
     * @param callback_ the callback function with first argument the "this" pointer for the View object of the class that implemented the callback,
     * and the second argument is a pointer to the new view if the select_action is new_view
     * @param select_action_ the default select action for this object
     */
    Callback_menu_item(const char* text_, Mono_graphics& screen_, const Mono_mono_font& font_, View* callback_view_, void (*callback_)(View*, View**),
    View::Select_result select_action_=View::Select_result::no_op)  :
        Menu_item{text_, screen_, font_}, callback_view{callback_view_}, callback{callback_}, select_action{select_action_} {}

    /**
     * @brief handle the select action for this menu item
     *
     * @param view_ set *view_ to a pointer to the new view if select_action is new_view
     * @return select_action
     */
    virtual View::Select_result on_select(View** view_) { callback(callback_view, view_); return select_action; }

    /**
     * @brief Set the select action value for this Menu_item
     *
     * @param new_action the new value for select_action
     */
    void set_select_action(View::Select_result new_action) { select_action = new_action; }
protected:
    View* callback_view;
    void (*callback)(View* callback_view_);
    View::Select_result select_action;
};
}