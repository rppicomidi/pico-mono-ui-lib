/**
 * @file setting_string_enum.h
 *
 * This class implements 0-based enum values as indices into a vector
 * of string values. The string value is serialized to JSON and
 * deserialized from JSON. The Text_item_chooser_menu class is a good
 * way to choose a value from the vector of string values. 
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
 */#pragma once
#include <cstdint>
#include <string>
#include <initializer_list>
#include <vector>
#include <cassert>
#include "parson.h"
namespace rppicomidi
{
class Setting_string_enum
{
public:
    Setting_string_enum()=delete;
    Setting_string_enum(std::string name_, std::initializer_list<const std::string> args) : name{name_}
    {
        for (auto& arg: args) {
            value_list.push_back(arg);
        }
        assert(value_list.size() > 0);
        set_default();
    }
    bool operator==(const std::string& rhs) { return (*current) == rhs; }
    void set_default() {current = value_list.begin(); }
    bool set(const std::string& newval)
    {
        bool success = false;
        for (auto it = value_list.begin(); !success && it != value_list.end(); it++) {
            if ((*it) == newval) {
                success = true;
                current = it;
            }
        }
        return success;
    }

    bool set(size_t idx)
    {
        if (idx < value_list.size()) {
            current = value_list.begin()+idx;
            return true;
        }
        return false;
    }

    bool set(std::string& strval)
    {
        size_t idx = 0;
        for (idx = 0; idx < value_list.size() && value_list[idx] != strval; idx++)
        {}
        if (idx < value_list.size())
            return set(idx);
        return false;
    }

    void get(std::string& setting) {setting = (*current); }

    int get_ivalue() {return current - value_list.begin(); }

    const std::vector<std::string>* get_all_possible_values() const { return &value_list; }
    const std::string& incr(int delta) {
        int idx = current - value_list.begin() + delta;
        if (idx < 0)
            current = value_list.begin();
        else if (idx >= (int)value_list.size())
            current = value_list.end()-1;
        else
            current = value_list.begin()+idx;
        return (*current);
    }

    void serialize(JSON_Object *root_object)
    {
        json_object_set_string(root_object, name.c_str(), (*current).c_str());
    }

    bool deserialize(JSON_Object *root_object)
    {
        if (json_object_has_value_of_type(root_object, name.c_str(), JSONString)) {
            const char* val = json_object_get_string(root_object, name.c_str());
            if (val == nullptr) {
                printf("unexpected nullptr for setting %s\r\n", name.c_str());
            }
            else if (!set(std::string{val})) {
                printf("%s is not legal for setting %s\r\n", val, name.c_str());
                return false;
            }
        }
        else {
            printf("Could not parse %s from settings\r\n", name.c_str());
            return false;
        }
        return true;
    }
private:
    std::string name;
    std::vector<std::string> value_list;
    std::vector<std::string>::iterator current;
};
}