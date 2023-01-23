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
#pragma once
#include <string>
#include "view.h"
namespace rppicomidi {
class Text_entry_box : public View
{
public:
    Text_entry_box()=delete;
    ~Text_entry_box()=default;
    /**
     * @brief Construct a new Text_entry_box object
     * 
     * @param screen_ the Mono_graphics screen to draw on
     * @param title_ a string to label the text entry box. If it is empty,
     * then the box takes the whole screen
     * @param max_chars_ the maximum number of characters accepted in the text box
     * @param illegal_chars_ a string containing characters that will be ignored when typed
     * @param done_cb_ is called when this view exits with the bool argument set true if select was press to exit
     * @param hide_typing_ if true, display '*' instead of the typed character; e.g., for passwords
     */
    Text_entry_box(Mono_graphics& screen_, const char* title_, size_t max_chars_,
        const std::string illegal_chars_, View* cb_context_, void (*done_cb_)(View*, bool ), bool hide_typing_=false);
    void draw();
    void entry();
    void exit();
    /**
     * @brief handle pressing Select, which means accept the input, so text_ok is true;
     * 
     * @param new_view_ unused
     * @return Select_result will be exit_view
     */
    Select_result on_select(View**);
    //void on_increment(uint32_t delta, bool is_shifted) final;
    //void on_decrement(uint32_t delta, bool is_shifted) final;
    void on_left(uint32_t delta, bool is_shifted);
    void on_right(uint32_t delta, bool is_shifted);
    void on_key(uint8_t key_code, uint8_t modifiers, bool pressed);
    /**
     * @brief Get the text typed into the Text_entry_box so far
     * 
     * @return const char* a pointer to the text
     */
    const char *get_text() { return text_typed.c_str(); }

    /**
     * @brief Set the text (e.g., initial text before displaying the box)
     *
     * @param text a pointer to the text to display
     */
    void set_text(const char* text);

    /**
     * @brief Set the cursor to the end of the text_typed string
     */
    void set_cursor_to_end();
protected:
    void delete_char_at_cursor_position();
    const Mono_mono_font font;
    const std::string title;
    const uint8_t y;
    size_t max_chars;
    const std::string illegal_chars;
    View* cb_context;
    void (*done_cb)(View*, bool );
    bool hide_typing;
    size_t cursor_position;
    const uint8_t chars_per_line;
    const uint8_t max_lines;
    bool overwrite_mode;
    bool text_ok;
    std::string text_typed;
};
}