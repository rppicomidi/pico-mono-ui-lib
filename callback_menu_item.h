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
    Callback_menu_item(const char* text_, Mono_graphics& screen_, const Mono_mono_font& font_, View* callback_view_, void (*callback_)(View*),
    View::Select_result select_action_=View::Select_result::no_op)  :
        Menu_item{text_, screen_, font_}, callback_view{callback_view_}, callback{callback_}, select_action{select_action_} {}

    virtual View::Select_result on_select(View**) { callback(callback_view); return select_action; }
    void set_select_action(View::Select_result new_action) { select_action = new_action; }
protected:
    View* callback_view;
    void (*callback)(View* callback_view_);
    View::Select_result select_action;
};
}