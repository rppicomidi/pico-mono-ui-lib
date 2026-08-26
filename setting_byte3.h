/**
 * @file setting_byte3.h
 *
 * This template class implements an array of number pairs suitable for mapping
 * one number to another number. The setting values can be serialized to JSON
 * and deserialized from JSON. The Byte3_spinner_menu_item is a good class to
 * use to adjust this value
 *
 * MIT License
 *
 * Copyright (c) 2026 rppicomidi
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
#include <cstdint>
#include <vector>
#include <array>
#include <cassert>
#include "parson.h"
namespace rppicomidi
{
class Setting_byte3
{
public:
    Setting_byte3(const char* name_, std::array<uint8_t, 3>minval_, std::array<uint8_t,3> maxval_) :
        name{name_}
    {
        for (int idx = 0; idx < 3; idx++) {
            minval[idx] = minval_[idx];
            maxval[idx] = maxval_[idx];
        }
        set_default();
    }
    Setting_byte3()=delete;

    virtual ~Setting_byte3()=default;

    /**
     * @brief Get the maximum value for the specfied byte
     * 
     * @return the maximum value for the specified byte
     */
    uint8_t get_max(int idx) { return maxval[idx]; }

    /**
     * @brief Set the setting value to default
     */
    void set_default() { for (int idx=0;idx < 3; idx++) byte3[idx] = maxval[idx]; }
    
    bool set(size_t byte3_idx, uint8_t value)
    {
        bool result = false;
        if (byte3_idx < byte3.size()) {
            if (value <= maxval[byte3_idx]) {
                result = true;
                byte3[byte3_idx] = value;
            }
        }
        return result;
    }

    uint8_t get(size_t byte3_idx)
    {
        uint8_t val = 255; // an invalid value
        if (byte3_idx < byte3.size()) {
            val = byte3[byte3_idx];
        }
        return val;
    }

    /**
     * @brief Get the maximum value for the specfied byte
     * 
     * @return the maximum value for the specified byte
     */
    uint8_t get_max(size_t byte3_idx)
    {
        uint8_t val = 0; // should be invalid for a maximum
        if (byte3_idx < byte3.size()) {
            val = maxval[byte3_idx];
        }
        return val;
    }

    /**
     * @brief Get the minimum value for the specfied byte
     * 
     * @return the minimum value for the specified byte
     */
    uint8_t get_min(size_t byte3_idx)
    {
        uint8_t val = 255; // should be invalid for a minimum
        if (byte3_idx < byte3.size()) {
            val = minval[byte3_idx];
        }
        return val;
    }

    /**
     * @brief add delta to the element at byte3[byte3_idx] and
     * keep the value between 0 and maxval[byte3_idx], inclusive
     * 
     * @param byte3_idx the index of the byte to change
     * @param delta the ammount to increment (or if negative, to decrement) the setting
     * @return get_max()+1 if byte3_idx or element_idx are out of range. Otherwise,
     * return byte3_idx[byte3_idx][element_idx]+delta limited to the valid valid range
     */
    virtual uint8_t incr(size_t byte3_idx, int delta)
    {
        uint8_t number = 255; // an invalid number, hopefully
        if (byte3_idx < byte3.size()) {
            uint8_t prev = byte3[byte3_idx]; // detect unsigned number wrapping
            number = prev + delta;
            if (delta < 0 && (number > prev || number < minval[byte3_idx])) {
                number = minval[byte3_idx];
            }
            else if (delta > 0 && (number < prev || number > maxval[byte3_idx])) {
                number = maxval[byte3_idx];
            }
            byte3[byte3_idx] = number;
        }
        return number;
    }

    /**
     * @brief Get the name of this setting
     * @return const char* the name of this setting
     */
    const char* get_name() {return name; }

    /**
     * @brief add byte3 vector to the JSON root object
     * 
     * @param root_object the JSON root object, created from the JSON root value
     * as follows for Setting_number object foo
     * JSON_Value *root_value = json_value_init_object();
     * JSON_Object *root_object = json_value_get_object(root_value);
     * foo.serialize(root_object);
     * 
     * @note To get the final serialization string you must call 
     * json_serialize_to_string(root_value)
     */
    void serialize(JSON_Object *root_object)
    {
        JSON_Value* byte3_json_value = json_value_init_array();
        assert(byte3_json_value);
        JSON_Array* byte3_json = json_value_get_array(byte3_json_value);
        assert(byte3_json);
        for (auto& element: byte3) {
            json_array_append_number(byte3_json, element);
        }
        JSON_Value* maxval_json_value = json_value_init_array();
        assert(maxval_json_value);
        JSON_Array* maxval_json = json_value_get_array(maxval_json_value);
        assert(maxval_json);
        for (auto& element: maxval) {
            json_array_append_number(maxval_json, element);
        }
        JSON_Value* minval_json_value = json_value_init_array();
        assert(minval_json_value);
        JSON_Array* minval_json = json_value_get_array(minval_json_value);
        assert(minval_json);
        for (auto& element: minval) {
            json_array_append_number(minval_json, element);
        }
        JSON_Value *byte3_settings_value = json_value_init_object();
        JSON_Object *byte3_settings_object = json_value_get_object(byte3_settings_value);
        json_object_set_value(byte3_settings_object, "byte3", json_array_get_wrapping_value(byte3_json));
        json_object_set_value(byte3_settings_object, "maxval", json_array_get_wrapping_value(maxval_json));
        json_object_set_value(byte3_settings_object, "minval", json_array_get_wrapping_value(minval_json));
        json_object_set_value(root_object, name, byte3_settings_value);
    }

    /**
     * @brief extract the value from the setting with the name of this object.
     * 
     * @param root_object the JSON root object, created from the root value
     * as follows for Setting_number object foo:
     *  JSON_Value *root_value = json_parse_string(settings_str);
     *  JSON_Object *root_object = NULL;
     *  if (root_value && json_value_get_type(root_value) == JSONObject) {
     *      root_object = json_value_get_object(root_value);
     *      if (!foo.deserialize(root_object) {
     *          // TODO handle error
     *      }
     *  }
     * @return true if val was successfully extracted from the string
     * @return false if the setting name was not found or the setting string could not be parsed or
     * the setting value was out of range
     */
    bool deserialize(JSON_Object *root_object)
    {
        set_default();
        if (json_object_has_value_of_type(root_object, name, JSONObject)) {
            JSON_Value* byte3_settings_value = json_object_get_value(root_object, name);
            JSON_Object* byte3_settings_object = json_value_get_object(byte3_settings_value);
            JSON_Array* byte3_json = json_object_get_array(byte3_settings_object, "byte3");
            if (byte3_json == nullptr) {
                printf("could not find array object 'byte3' in the '%s' object\r\n",name);
                return false;
            }
            JSON_Array* maxval_json = json_object_get_array(byte3_settings_object, "maxval");
            if (maxval_json == nullptr) {
                printf("could not find array object 'maxval' in the '%s' object\r\n", name);
                return false;
            }
            JSON_Array* minval_json = json_object_get_array(byte3_settings_object, "minval");
            if (maxval_json == nullptr) {
                printf("could not find array object 'minval' in the '%s' object\r\n", name);
                return false;
            }
            for (size_t jdx = 0; jdx < 3; jdx++) {
                JSON_Value* val = json_array_get_value(byte3_json, jdx);
                if (val == nullptr) {
                    set_default();
                    return false;
                }
                if (json_value_get_type(val) == JSONNumber) {
                    byte3[jdx] = json_array_get_number(byte3_json, jdx);
                }
                else {
                    printf("byte3 array value not a number\r\n");
                    set_default();
                    return false;
                }
                val = json_array_get_value(maxval_json, jdx);
                if (val == nullptr) {
                    set_default();
                    return false;
                }
                if (json_value_get_type(val) == JSONNumber) {
                    maxval[jdx] = json_array_get_number(maxval_json, jdx);
                }
                val = json_array_get_value(minval_json, jdx);
                if (val == nullptr) {
                    set_default();
                    return false;
                }
                if (json_value_get_type(val) == JSONNumber) {
                    minval[jdx] = json_array_get_number(minval_json, jdx);
                }
                else {
                    printf("maxval array value not a number\r\n");
                    set_default();
                    return false;
                }
            }
        }
        else {
            printf("Could not parse '%s' object from settings\r\n", name);
            return false;
        }
        return true;
    }
private:
    const char* name;
    std::array<uint8_t,3> maxval;
    std::array<uint8_t,3> minval;
    std::array<uint8_t,3> byte3;
};
}