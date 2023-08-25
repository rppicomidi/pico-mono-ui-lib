/**
 * @file view_manager.cpp
 * @brief this class manages what view is currently visible and dispatches
 * UI events like rotary encoder twists, button presses, etc. to the the
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
#include "view_manager.h"

void rppicomidi::View_manager::push_view(View* new_view)
{
    view_stack.push_back(new_view);
    current_view = view_stack.end() - 1;
    (*current_view)->entry();
    auto result = (*current_view)->set_has_focus(true);
    #if CFG_TUSB_DEBUG < 1
    (void)result;
    #endif
    assert(result != View::Select_result::exit_view); //pushing a new view view should not immediately exit
    (*current_view)->draw();
}

void rppicomidi::View_manager::pop_view()
{
    if (current_view != view_stack.begin() && current_view != view_stack.end()) {
        (*current_view)->exit();
        view_stack.pop_back();
        current_view = view_stack.end() - 1;
        (*current_view)->entry();
        (*current_view)->draw();
    }
    
}

void rppicomidi::View_manager::go_home()
{
    if (current_view != view_stack.begin() && current_view != view_stack.end()) {
        (*current_view)->exit();
        do {
            view_stack.pop_back();
            current_view = view_stack.end() - 1;
        } while (current_view != view_stack.begin());
        (*current_view)->entry();
        (*current_view)->draw();
    }
}

void rppicomidi::View_manager::on_select()
{
    if (current_view != view_stack.end()) {
        View* new_view_;
        View::Select_result result = (*current_view)->on_select(&new_view_);
        if (result == View::Select_result::exit_view) {
            pop_view();
        }
        else if (result == View::Select_result::new_view) {
            push_view(new_view_);
        }
    }
}

void rppicomidi::View_manager::on_back()
{
    if (current_view != view_stack.end()) {
        View* new_view_;
        View::Select_result result = (*current_view)->on_back(&new_view_);
        if (result == View::Select_result::exit_view) {
            pop_view();
        }
        else if (result == View::Select_result::new_view) {
            push_view(new_view_);
        }
    }
}