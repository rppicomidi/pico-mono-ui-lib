/**
 * @file menu.h
 * @brief this class describes the interface for a text-based menu.
 * 
 * A menu has a vector of pointers to Menu_item objects. The Menu_item
 * objects must be allocated with operator new because the Menu class deletes
 * every Menu_item in its vector in the Menu class destructor. The Menu object
 * specifies the font for rendering all Menu_item object. A Menu has a bounding
 * rectangle that is a multiple of the Menu_item font height and is the
 * width of the screen. A Menu will only display as many Menu_item objects
 * that can fit within the bounding rectangle. If there are more Menu_item
 * objects in the Menu than can be seen at once, the Menu will display a
 * windowed view of the list of Menu_item objects and a vertical "scroll
 * bar" on the right side of the screen that indicates what window of the
 * total number of Menu_item objects are visible at the time.
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
#include "menu_item.h"
#include <vector>
namespace rppicomidi {
class Menu : public View {
public:
    Menu(Mono_graphics& screen_, uint8_t y_, const Mono_mono_font& menu_font_) : 
        View(screen_, Rectangle{0,y_,screen_.get_screen_width(), static_cast<uint8_t>(screen_.get_screen_height() - y_)}),
        menu_font{menu_font_},
        scroll_offset{0}
    {
        current_item = items.end();
        max_visisble_items = view_rect.height / menu_font.height;
    }
    virtual ~Menu();
    virtual void draw();
    virtual void entry() {
        has_focus = true;
        for (auto& item: items) {
            item->exit();
        }
    }
    virtual void exit() {
        has_focus = false;
        for (auto& item: items) {
            item->exit();
        }
    }
    virtual void add_menu_item(Menu_item* item_) {
        items.push_back(item_);
        current_item = items.begin();
        scroll_offset = 0;
    }

    /**
     * @brief free the memory allocated to the current menu item
     * and then erase the pointer to the item from the menu's items list.
     *
     * @note do not call this function unless the item being erased was
     * allocated by new.
     */
    virtual void erase_current_item()
    {
        int idx = get_current_item_idx(); // keep offset of the current menu item
        delete (*current_item); // free the menu item that was dynamically allocated
        items.erase(current_item); // remove the current item from the memu
        // update the menu's current item
        if (idx < static_cast<int>(items.size())) {
            current_item = items.begin()+idx;
        }
        else {
            current_item = items.end();
        }
    }

    /**
     * @brief erase all menu items
     */
    virtual void clear()
    {
        for (auto& item:items)
            delete item;
        items.clear();
        current_item = items.end();
    }

    virtual void insert_menu_item_before_current(Menu_item * item_)
    {
        int idx = get_current_item_idx(); // keep offset of the current menu item
        if (idx == -1) {
            // list is empty
            add_menu_item(item_);
        }
        else {
            (*current_item)->set_highlighted(false);
            items.insert(current_item, item_);
            // inserting invalidates the iterator
            current_item = items.begin()+idx; // The inserted item is now current_item
        }
    }
    virtual const char* get_menu_item_text(int idx) {if (idx >=0 && idx < (int)items.size()) return items[idx]->get_text(); return nullptr; }

    virtual void set_menu_item_text(int idx, const char* text_) {if (idx >=0 && idx < (int)items.size()) items[idx]->set_text(text_);}
    virtual Select_result on_select(View** new_view);

    virtual void on_increment(uint32_t delta, bool is_shifted);

    virtual void on_decrement(uint32_t delta, bool is_shifted);

    virtual void on_left(uint32_t delta, bool is_shifted) {if (current_item != items.end()) (*current_item)->on_left(delta, is_shifted); }
    virtual void on_right(uint32_t delta, bool is_shifted) {if (current_item != items.end()) (*current_item)->on_right(delta, is_shifted); }

    virtual void on_key(uint32_t key_code, bool pressed) {(void)key_code; (void)pressed;}

    size_t get_num_items() {return items.size(); }

    /**
     * @brief Get the current item idx
     * 
     * @return -1 if the list of menu items is empty. Otherwise, return the menu item index from 0.
     */
    int get_current_item_idx()
    {
        if (current_item == items.end())
            return -1;
        return current_item-items.begin();
    }

    int set_current_item_idx(int idx);

    Menu_item* get_current_item()
    {
        if (current_item == items.end())
            return nullptr;
        return *current_item;
    }
protected:
    const Mono_mono_font& menu_font;
    std::vector<Menu_item*> items;
    std::vector<Menu_item*>::iterator current_item;
    uint8_t max_visisble_items;
    /* scroll_offset is the offset from items.begin().
     * If items.begin()+scroll_offset > current_item,
     * then need to decrement scroll_offset and redraw.
     * If items.begin()+scroll_offset+max_visibile_items <= current_item,
     * then need to increment scroll_offset and redraw.
     */
    int scroll_offset; 
};
}