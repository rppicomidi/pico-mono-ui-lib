#include "view_launch_menu_item.h"

rppicomidi::View::Select_result rppicomidi::View_launch_menu_item::on_select(View** new_view_)
{
    *new_view_ = &new_view;
    return View::new_view;
}