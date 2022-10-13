#include "text_item_chooser_menu.h"

rppicomidi::Text_item_chooser_menu::Text_item_chooser_menu(Mono_graphics& screen_, uint8_t y_, View* view_, 
        void (*selection_callback_)(View* view, int& selected_idx)) :
            Menu{screen_, y_, screen_.get_font_12()}, callback_view{view_}, selection_callback{selection_callback_}
            
{

}

rppicomidi::View::Select_result rppicomidi::Text_item_chooser_menu::on_select(rppicomidi::View**)
{
    int idx = get_current_item_idx();
    selection_callback(callback_view, idx);
    return View::Select_result::exit_view;
}