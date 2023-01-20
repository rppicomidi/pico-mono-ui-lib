/**
 * @file text_item_chooser_menu.cpp
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

#include "text_item_chooser_menu.h"

rppicomidi::Text_item_chooser_menu::Text_item_chooser_menu(Mono_graphics& screen_, uint8_t y_, View* view_, 
        void (*selection_callback_)(View* view, int selected_idx)) :
            Menu{screen_, y_, screen_.get_font_12()}, callback_view{view_}, selection_callback{selection_callback_}
            
{

}

rppicomidi::View::Select_result rppicomidi::Text_item_chooser_menu::on_select(rppicomidi::View**)
{
    int idx = get_current_item_idx();
    selection_callback(callback_view, idx);
    return View::Select_result::exit_view;
}