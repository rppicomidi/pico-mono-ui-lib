#pragma once
#include "menu_item.h"
#include "view.h"

namespace rppicomidi
{
class View_launch_menu_item : public Menu_item
{
public:
    View_launch_menu_item(View& new_view_, const char* text_, Mono_graphics& screen_, const Mono_mono_font& font_) :
        Menu_item{text_, screen_, font_}, new_view{new_view_} {}
    View& get_new_view() { return new_view; }
    View::Select_result on_select(View** new_view_);
protected:
    View& new_view;
};
}