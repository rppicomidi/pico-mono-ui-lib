cmake_minimum_required(VERSION 3.13)

add_library(ui_menu INTERFACE)
target_sources(ui_menu INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/menu.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view_launch_menu_item.cpp
)
target_include_directories(ui_menu INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(ui_menu INTERFACE mono_graphics_lib pico_stdlib)

add_library(ui_view_manager INTERFACE)
target_sources(ui_view_manager INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/view_manager.cpp
)
target_include_directories(ui_view_manager INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(ui_view_manager INTERFACE mono_graphics_lib pico_stdlib)

add_library(ui_nav_buttons INTERFACE)
target_sources(ui_nav_buttons INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/nav_buttons.cpp
)
target_include_directories(ui_nav_buttons INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(ui_nav_buttons INTERFACE pico_stdlib ui_view_manager)

add_library(ui_text_item_chooser INTERFACE)
target_sources(ui_text_item_chooser INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/text_item_chooser_menu.cpp
)
target_include_directories(ui_text_item_chooser INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(ui_text_item_chooser INTERFACE pico_stdlib ui_menu ui_view_manager)