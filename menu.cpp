#include "menu.h"

rppicomidi::Menu::~Menu()
{
    for (auto& item:items)
        delete item;
}

void rppicomidi::Menu::on_decrement(uint32_t delta, bool is_shifted)
{
    if (delta > 0 && current_item != items.end()) {
        // then list is not empty and delta > 0
        if (has_focus) {
            if (is_shifted)
                delta += 5; // down arrow becomes page down
            // then navigate to the next visible item if not already
            // at the end of the list (TODO: implement scrolling)
            if (delta > 1) {
                // make sure we are not navigating past the end of the list
                uint32_t new_delta = 1;
                while (new_delta < delta && current_item+new_delta != items.end())
                    ++new_delta;
                if (current_item+new_delta == items.end()) {
                    if (new_delta > 1) {
                        delta = new_delta - 1; // can still advance
                    }
                    else {
                        delta = 1;  // already at the end of the list
                    }
                } // otherwise, the given delta is fine
            }
            if (current_item+delta != items.end() ) {
                auto next_item = current_item+delta;
                while ((*next_item)->is_hidden() && next_item + 1 != items.end()) {
                    next_item++;
                }
                if (current_item != next_item) {
                    if ((*next_item)->is_hidden()) {
                        if (delta > 1) {
                            // can't advance by delta or more, but maybe by less than delta
                            --delta;
                            next_item = current_item + delta;
                            while (delta > 0 && (*next_item)->is_hidden()) {
                                --delta;
                                next_item = current_item + delta;
                            }
                            if (delta > 0) {
                                (*current_item)->set_highlighted(false);
                                while(next_item >= items.begin()+scroll_offset+max_visisble_items) {
                                    ++scroll_offset;
                                }
                                current_item = next_item;
                                draw();
                            }
                        }
                    }
                    else {
                        (*current_item)->set_highlighted(false);
                        while(next_item >= items.begin()+scroll_offset+max_visisble_items) {
                            ++scroll_offset;
                        }
                        current_item = next_item;
                        draw();
                    }
                } 
            }
        }
        else {
            (*current_item)->on_decrement(delta, is_shifted);
        }
    }
}

void rppicomidi::Menu::on_increment(uint32_t delta, bool is_shifted)
{
    if (delta > 0 && current_item != items.end()) {
        // then list is not empty and delta > 0
        if (has_focus) {
            if (is_shifted)
                delta += 5; // down arrow becomes page down
            // then navigate to the next visible item if not
            // already at the beginning of the list (TODO: implement scrolling)
            if (delta > 1) {

                // make sure we are not navigating before the beginning of the list
                uint32_t new_delta = 0;
                while (new_delta < delta && current_item - new_delta != items.begin()) {
                    ++new_delta;
                }
                if (new_delta > 0)
                    delta = new_delta;
            }
            if (current_item != items.begin()) {
                auto next_item = current_item-delta;
                while ((*next_item)->is_hidden() && next_item != items.begin()) {
                    next_item--;
                }
                if (current_item != next_item) {
                    if ((*next_item)->is_hidden()) {
                        if (delta > 1) {
                            // can't advance by delta or more, but maybe by less than delta
                            --delta;
                            next_item = current_item - delta;
                            while (delta > 0 && (*next_item)->is_hidden()) {
                                --delta;
                                next_item = current_item - delta;
                            }
                            if (delta > 0) {
                                (*current_item)->set_highlighted(false);
                                while(scroll_offset && next_item < items.begin()+scroll_offset) {
                                    --scroll_offset;
                                }
                                current_item = next_item;
                                draw();
                            }
                        }
                    }
                    else {
                        (*current_item)->set_highlighted(false);
                        while(scroll_offset && next_item < items.begin()+scroll_offset) {
                            --scroll_offset;
                        }
                        current_item = next_item;
                        draw();
                    }
                }
            }
        }
        else {
            (*current_item)->on_increment(delta, is_shifted);
        }
    }
}

rppicomidi::Menu::Select_result rppicomidi::Menu::on_select(View** new_view)
{
    Select_result result = View::no_op;
    if (current_item != items.end() && (*current_item)->is_highlighted()) {
        result = (*current_item)->on_select(new_view);
        if (result == Select_result::take_focus) {
            has_focus = false;
        }
        else if (result == Select_result::give_focus) {
            has_focus = true;
        }
    }
    return result;
}

void rppicomidi::Menu::draw()
{
    // Clear the bounding rectangle
    screen.draw_rectangle(view_rect, Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ZERO);
    if (items.size() == 0)
        return;
    // figure out items statistics for the vertical scroll bar, in case it's required
    int invisible_items = 0;
    int items_drawn = 0;
    int visible_items = 0;
    int scrolled_past_items = 0;
    int current_offset = 0;
    for (auto& item : items) {
        item->exit();
        if (!item->is_hidden()) {
            ++visible_items;
            if (current_offset < scroll_offset)
                ++scrolled_past_items;
        }
        ++current_offset;
    }
    (*current_item)->set_highlighted(true);

    for (auto item=items.begin()+scroll_offset; item != items.end() && item < items.begin()+scroll_offset+max_visisble_items-invisible_items; item++) {        
        if (!(*item)->is_hidden()) {
            (*item)->entry();
            (*item)->draw(items_drawn*menu_font.height+view_rect.y_upper_left);
            ++items_drawn;
        }
        else {
            ++invisible_items;
        }
    }
    if (items_drawn < visible_items) {
        // show a vertical scroll bar
        uint8_t scroll_bar_width = 4;
        uint8_t scroll_bar_x = screen.get_screen_width()-scroll_bar_width;
        screen.draw_rectangle(scroll_bar_x, view_rect.y_upper_left, scroll_bar_width, view_rect.height,
            Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
        uint8_t rect_height = items_drawn * view_rect.height / visible_items;
        uint8_t rect_y = (scrolled_past_items * view_rect.height / visible_items) + view_rect.y_upper_left;
        screen.draw_rectangle(scroll_bar_x, rect_y, scroll_bar_width, rect_height,
            Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ONE);

    }
}
