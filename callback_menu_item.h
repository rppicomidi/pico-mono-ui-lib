#pragma once
#include "menu_item.h"
#include "view.h"
namespace rppicomidi
{
class Callback_menu_item : public Menu_item
{
public:
    Callback_menu_item(const char* text_, Mono_graphics& screen_, const Mono_mono_font& font_, View* callback_view_, void (*callback_)(View*))  :
        Menu_item{text_, screen_, font_}, callback_view{callback_view_}, callback{callback_} {}

    virtual View::Select_result on_select(View**) { callback(callback_view); return View::Select_result::no_op; }
protected:
    View* callback_view;
    void (*callback)(View* callback_view_);
};
}