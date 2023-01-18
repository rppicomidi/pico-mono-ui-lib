/**
 * MIT License
 *
 * Copyright (c) 2023 rppicomidi
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
#include "text_entry_box.h"
#include "hid_keyboard.h"

rppicomidi::Text_entry_box::Text_entry_box(Mono_graphics& screen_, const char* title_, size_t max_chars_,
        const std::string illegal_chars_, View* cb_context_, void (*done_cb_)(View*, bool ), bool hide_typing_) :
    View{screen_, screen_.get_clip_rect()}, font{screen_.get_font_12()}, title{title_},
    y{(uint8_t)(title.size() ? font.height + 4: 0)}, max_chars{max_chars_},
    illegal_chars{illegal_chars_}, cb_context{cb_context_}, done_cb{done_cb_}, hide_typing{hide_typing_},
    cursor_position{0}, chars_per_line{(uint8_t)(screen.get_screen_width()/font.width)},
    max_lines{(uint8_t)((max_chars/ chars_per_line) + ((max_chars % chars_per_line) != 0))},
    overwrite_mode{false}, text_ok{false}
{

}

void rppicomidi::Text_entry_box::set_cursor_to_end()
{
    if (text_typed.size() == max_chars)
        cursor_position = max_chars-1;
    else
        cursor_position = text_typed.size();
}

void rppicomidi::Text_entry_box::set_text(const char* text)
{
    text_typed = std::string(text);
    if (text_typed.size() > max_chars)
        text_typed = text_typed.substr(0, max_chars);
}

void rppicomidi::Text_entry_box::draw()
{
    uint8_t line = y;
    uint8_t col = 0;
    size_t pos = 0;
    // Clear the text box drawing area
    screen.draw_rectangle(0, y, screen.get_screen_width(), font.height*max_lines,
        Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ZERO);
    // draw the previously typed text with the curson position highlighted
    for (auto ch: text_typed) {
        Pixel_state fg, bg;
        if (pos++ == cursor_position) {
            fg = Pixel_state::PIXEL_ZERO;
            bg = Pixel_state::PIXEL_ONE;
        }
        else {
            fg = Pixel_state::PIXEL_ONE;
            bg = Pixel_state::PIXEL_ZERO;
        }
        screen.draw_character(font, col * font.width, line, ch, fg, bg);
        if (++col == chars_per_line) {
            col = 0;
            line += font.height;
        }
    }
    if (cursor_position >= text_typed.size() && cursor_position < max_chars) {
        screen.draw_character(font, col * font.width, line, ' ', Pixel_state::PIXEL_ZERO, Pixel_state::PIXEL_ONE);
    }
}

void rppicomidi::Text_entry_box::entry()
{
    text_ok = false; // in case someone exits the text box without pressing Select
    set_cursor_to_end();
    screen.clear_canvas();
    if (title.size())
        screen.center_string(font, title.c_str(),0);
}

void rppicomidi::Text_entry_box::on_left(uint32_t, bool)
{
    if (cursor_position > 0) {
        --cursor_position;
    }
    draw();
}

void rppicomidi::Text_entry_box::on_right(uint32_t, bool)
{
    if (cursor_position <= text_typed.size()) {
        if (++cursor_position == max_chars) {
            cursor_position = max_chars;
        }
    }
    draw();
}

void rppicomidi::Text_entry_box::delete_char_at_cursor_position()
{
    size_t text_typed_size = text_typed.size();
    if (cursor_position < text_typed_size) {
        if (cursor_position == 0) {
            text_typed = text_typed.substr(1);
        }
        else if (cursor_position == text_typed_size - 1) {
            // delete from the end
            text_typed = text_typed.substr(0, text_typed_size - 1);
        }
        else {
            text_typed = text_typed.substr(0, cursor_position) + text_typed.substr(cursor_position + 1);
        }
        draw();
    }
}

void rppicomidi::Text_entry_box::on_key(uint8_t key_code, uint8_t modifiers, bool pressed)
{
    if (pressed) {
        switch(key_code) {
        case HID_KEY_DELETE:
            if (cursor_position < text_typed.size()) {
                delete_char_at_cursor_position();
                draw();
            }
            break;
        case HID_KEY_BACKSPACE:
            if (cursor_position > 0) {
                --cursor_position;
                delete_char_at_cursor_position();
                draw();
            }
            break;
        case HID_KEY_TAB:
            // ignore the tab key
            break;
        case HID_KEY_HOME:
            cursor_position = 0;
            draw();
            break;
        case HID_KEY_END:
            cursor_position = text_typed.size();
            if (cursor_position >= max_chars) {
                cursor_position = max_chars - 1;
            }
            break;
        case HID_KEY_INSERT:
            overwrite_mode = !overwrite_mode;
            break;
        default:
        {
            char ch[2] = {'\0','\0'};
            ch[0] = Hid_keyboard::translate_keycode(key_code, modifiers);
            if (ch[0]) {
                if (overwrite_mode) {
                    text_typed.at(cursor_position) = ch[0];
                }
                else if (text_typed.size() < max_chars) {
                    if (cursor_position == 0) {
                        // insert from the begining                        
                        text_typed = std::string(ch) + text_typed;
                    }
                    else if (cursor_position == text_typed.size()) {
                        // append to the end
                        text_typed += std::string(ch);
                    }
                    else {
                        // insert in the middle of the string
                        text_typed = text_typed.substr(0, cursor_position)+ std::string(ch) + text_typed.substr(cursor_position);
                    }
                    ++cursor_position;
                    draw();
                }
            }
        }
            break;
        }
    }
}