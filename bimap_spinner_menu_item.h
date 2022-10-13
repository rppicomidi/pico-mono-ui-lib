/**
 * @file bimap_spinner_menu_item.h
 * @author rppicomidi
 * @brief This template class allows you to edit two integrals
 * values intended for Setting_bimap objects
 * 
 * Format of the menu item is 
 * "text" "first bimap value" "->" "second bimap value"
 * Examples:
 * CC:127->126
 * Note: 63->44 
 * Prog:  1->127
 *
 * @copyright Copyright (c) 2022 rppicomidi
 * 
 */
#pragma once
#include <type_traits>
#include <cstdio>
#include "view.h"
#include "menu_item.h"
#include "setting_number.h"
#include "mono_graphics_lib.h"
namespace rppicomidi
{
template<typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
class Bimap_spinner_menu_item : public Menu_item
{
public:
    Bimap_spinner_menu_item()=delete;
    /**
     * @brief Construct a new Bimap_spinner_menu_item object
     * 
     * @param text_ The text label for the int spinner value
     * @param screen_ The screen object that renders this menu item
     * @param font_  The font to render the label and value
     * @param bimap_idx_ The index in the Setting_bimap object to edit
     * @param ndigits_ The number of digits of the number field (include space for a sign digit if needed)
     * @param nhex_digits_ The number of digits of the number field in hex format
     * @param hex_format_ True to render the int value in hex format; false to render in base 10
     * @param get_fn_ The function to get the setting value
     * @param incr_fn_ The function to increment (or decrement, if the int delta is negative) the currently selected setting value
     * @param context_ A pointer to the class that contains the static get_fn and incr_fn cast to void*
     */
    Bimap_spinner_menu_item(const char* text_, Mono_graphics& screen_, const Mono_mono_font& font_,
        size_t bimap_idx_, int ndigits_, int nhex_digits_, bool hex_format_,
        T (*get_fn_)(void* context_, size_t bimap_idx, size_t element_idx),
        T (*incr_fn_)(void* context_, size_t bimap_idx, size_t element_idx, int delta), void* context_) :
        Menu_item{text_, screen_, font_},
        bimap_idx{bimap_idx_},
        ndigits{ndigits_}, nhex_digits{nhex_digits_}, hex_format{hex_format_},
        get_fn{get_fn_}, incr_fn{incr_fn_}, context{context_}
    {
        editing = 2;
        assert(ndigits >= nhex_digits);
    }

    virtual void redraw()
    {
        if (is_hidden())
            return;
        T first = get_fn(context, bimap_idx, 0);
        T second = get_fn(context, bimap_idx, 1);
        T max_val = get_fn(context, bimap_idx, 2) - 1; // The get() method returns the maximum value+1. Display max_val as all asterix
        char first_str[ndigits+1];
        char second_str[ndigits+1];
        if (hex_format) {
            snprintf(first_str, nhex_digits+1, "%0*X", nhex_digits, first);
            snprintf(second_str, nhex_digits+1, "%0*X", nhex_digits, second);
        }
        else {
            snprintf(first_str, ndigits+1, "%*d", ndigits, first);
            snprintf(second_str, ndigits+1, "%-*d", ndigits, second);
        }
        int disp_digits = hex_format? nhex_digits:ndigits;
        if (first == max_val) {
            for (int idx=0; idx < disp_digits; idx++)
                first_str[idx]='*';
            first_str[disp_digits] = '\0';
        }
        if (second == max_val) {
            for (int idx=0; idx < disp_digits; idx++)
                second_str[idx]='*';
            second_str[disp_digits] = '\0';
        }
        if (is_highlighted() && editing == 2) {
            // draw the label and both numbers in reverse text
            screen.draw_string(font, 0, last_draw_y, text, strlen(text), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            screen.draw_string(font, strlen(text)*font.width, last_draw_y, first_str, strlen(first_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            screen.draw_string(font, (strlen(text)+strlen(first_str))*font.width, last_draw_y, "->", 2, Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            screen.draw_string(font, (strlen(text)+strlen(first_str)+2)*font.width, last_draw_y, second_str, strlen(second_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
        }
        else {
            // The label text and the "->" are in normal text; the currently editing number is in reverse text
            screen.draw_string(font, 0, last_draw_y, text, strlen(text), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            screen.draw_string(font, (strlen(text)+strlen(first_str))*font.width, last_draw_y, "->", 2, Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            if (editing == 0) {
                screen.draw_string(font, strlen(text)*font.width, last_draw_y, first_str, strlen(first_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+2)*font.width, last_draw_y, second_str, strlen(second_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            }
            else if (editing == 1) {
                screen.draw_string(font, strlen(text)*font.width, last_draw_y, first_str, strlen(first_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+2)*font.width, last_draw_y, second_str, strlen(second_str), Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
            }
            else {
                screen.draw_string(font, strlen(text)*font.width, last_draw_y, first_str, strlen(first_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
                screen.draw_string(font, (strlen(text)+strlen(first_str)+2)*font.width, last_draw_y, second_str, strlen(second_str), Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
            }
        }
    }

    void entry() final
    {
        editing = 2;
    }

    void exit() final
    {
        editing = 2;
        last_draw_y=-1;
    }

    void on_increment(uint32_t delta, bool is_shifted) final
    {
        if (is_shifted)
            delta *= 10;

        incr((int)delta);
    }

    void on_decrement(uint32_t delta, bool is_shifted) final
    {
        if (is_shifted)
            delta *= 10;

        incr(-(int)delta);
    }

    virtual void on_left(uint32_t, bool)
    {
        if (editing == 1)
            editing = 0;
        redraw();
    }

    virtual void on_right(uint32_t, bool)
    {
        if (editing == 0)
            editing = 1;
        redraw();
    }

    virtual View::Select_result on_select(View**)
    {
        if (editing != 2) {
            editing = 2;
        }
        else {
            editing = 0;
        }
        redraw();
        if (editing != 2)
            return View::Select_result::take_focus;
        return View::Select_result::give_focus;
    }

    virtual size_t get_bimap_idx() const { return bimap_idx; }

    void set_display_hex(bool is_hex) { hex_format = is_hex; }
protected:
    void incr(int delta)
    {
        if (editing == 2)
            return;
        T oldval = get_fn(context, bimap_idx, editing);
        T newval = incr_fn(context, bimap_idx, editing, delta);
        if (oldval != newval)
            redraw();
    }
    size_t bimap_idx;
    int ndigits;
    int nhex_digits;
    bool hex_format;
    T (*get_fn)(void* context_, size_t bimap_idx, size_t element_idx);
    T (*incr_fn)(void* context_, size_t bimap_idx, size_t element_idx, int delta);
    void* context;
    size_t editing;       //<! 0 if editing the first value, 1 if editing the second value, 2 if not editing
};
}
