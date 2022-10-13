/**
 * @file text_item_chooser_screen.h
 * @brief this class describes a View for choosing an item from a menu
 * of text items. The up and down buttons allow navigating the menu. The
 * enter button calls the selection callback and exits the View. The Home or
 * Back button exits the View without calling the callback.
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
#include "mono_graphics_lib.h"
#include "view_manager.h"
#include "menu.h"
namespace rppicomidi
{
class Text_item_chooser_menu : public Menu
{
public:
    /**
     * @brief Construct a new Text_item_chooser_screen object
     * 
     * @param screen_ the screen that will render the Menu
     * @param y_ the number of pixels from the top of the screen to start drawing the menu
     * @param selection_callback_ the method that is called on press of the Enter button
     */
    Text_item_chooser_menu(Mono_graphics& screen_, uint8_t y_, View* callback_view_,
        void (*selection_callback_)(View* view, int& selected_idx));
    virtual View::Select_result on_select(View**);
protected:
    View* callback_view;
    void (*selection_callback)(View* view, int& selected_idx);
};
}