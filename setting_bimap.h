/**
 * @file setting_bimap.h
 *
 * This template class implements an array of number pairs suitable for mapping
 * one number to another number. The setting values can be serialized to JSON
 * and deserialized from JSON. The Bimap_spinner_menu_item is a good class to
 * use to adjust this value
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
 */
#pragma once
#include <type_traits>
#include <cstdint>
#include <vector>
#include <array>
#include <cassert>
#include "parson.h"
namespace rppicomidi
{
template<typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
class Setting_bimap
{
public:
    Setting_bimap(const char* name_, T minval_, T maxval_) :
        name{name_}, minval{minval_}, maxval{maxval_}
    {
        set_default();
    }
    Setting_bimap()=delete;

    virtual ~Setting_bimap()=default;
    /**
     * @brief Get the minimum value for this setting
     * 
     * @return T the minimum value for this setting
     */
    T get_min() { return minval; }

    void set_min(T minval_) { minval=minval_; }

    /**
     * @brief Get the maximum value for this setting
     * 
     * @return T the maximum value for this setting
     */
    T get_max() { return maxval; }

    void set_max(T maxval_) { maxval=maxval_; }

    /**
     * @brief Set the setting value to default
     */
    void set_default() { bimap.clear(); }

    /**
     * @brief find the index of the first instance of the key in the bimap array
     *
     * @param first is the value to remap
     * @param i_first is 0 for forward map, 1 for reverse map
     * @return -1 if first was not found, otherwise return the index where first was found
     */
    int find(T first, size_t i_first)
    {
        size_t idx;
        for (idx=0; idx < bimap.size() && first != bimap[idx][i_first]; idx++) {
        }
        return idx < bimap.size() ? idx: -1;
    }

    /**
     * @brief add a new pair of numbers to the end of the bimap vector
     * 
     * @param first the forward map key
     * @param second the forward map new value
     * @return the index where the item was added or -1 if
     * either first or second is out of range
     */
    int push_back(T first, T second)
    {
        if (first < get_min() || first > get_max())
            return -1;
        if (second < get_min() || second > get_max())
            return -1;
        bimap.push_back({first,second});
        return static_cast<int>(bimap.size())-1;
    }

    size_t size() {return bimap.size(); }
    
    bool set(size_t bimap_idx, size_t element_idx, T value)
    {
        bool result = false;
        if (bimap_idx < bimap.size() && element_idx < 2) {
            if (value >= get_min() && value <= get_max()) {
                result = true;
                bimap[bimap_idx][element_idx] = value;
            }
        }
        return result;
    }

    T get(size_t bimap_idx, size_t element_idx)
    {
        T val = get_max()+1; // an invalid value
        if (bimap_idx < bimap.size() && element_idx < 2) {
            val = bimap[bimap_idx][element_idx];
        }
        return val;
    }

    void erase(size_t idx)
    {
        if (idx < bimap.size()) {
            auto it = bimap.begin()+idx;
            bimap.erase(it);
        }
    }

    /**
     * @brief add delta to the element at bimap[bimap_idx][element_idx] and
     * keep the value between get_min() and get_max()
     * 
     * @param bimap_idx the index of the bi-directional mapping
     * @param element_idx is 0 for the forward map key and 1 for the reverse map key
     * @param delta the ammount to increment (or if negative, to decrement) the setting
     * @return get_max()+1 if bimap_idx or element_idx are out of range. Otherwise,
     * return bimap_idx[bimap_idx][element_idx]+delta limited to the valid valid range
     */
    virtual T incr(size_t bimap_idx, size_t element_idx, int delta)
    {
        T number = get_max()+1; // an invalid number
        if (bimap_idx < bimap.size() && element_idx < 2) {
            T prev = bimap[bimap_idx][element_idx]; // detect unsigned number wrapping
            number = prev + delta;
            if ((number > prev && delta < 0) || number < get_min()) {
                number = get_min();
            }
            else if ((number < prev && delta > 0) || number > get_max()) {
                number = get_max();
            }
            bimap[bimap_idx][element_idx] = number;
        }
        return number;
    }

    /**
     * @brief Get the name of this setting
     * @return const char* the name of this setting
     */
    const char* get_name() {return name; }

    /**
     * @brief add bimap vector to the JSON root object
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
        JSON_Value* bimap_json_value = json_value_init_array();
        assert(bimap_json_value);
        JSON_Array* bimap_json = json_value_get_array(bimap_json_value);
        assert(bimap_json);
        for (auto& element: bimap) {
            JSON_Value* element_json_value = json_value_init_array();
            assert(element_json_value);
            JSON_Array* element_json = json_value_get_array(element_json_value);
            assert(element_json);
            json_array_append_number(element_json, element[0]);
            json_array_append_number(element_json, element[1]);
            json_array_append_value(bimap_json, json_array_get_wrapping_value(element_json));
        }
        json_object_set_value(root_object, name, json_array_get_wrapping_value(bimap_json));
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
        if (json_object_has_value_of_type(root_object, name, JSONArray)) {
            JSON_Array* bimap_json = json_object_get_array(root_object, name);
            if (bimap_json == nullptr) {
                printf("could not find array object %s in the root object\r\n", name);
                return false;
            }
            size_t bimap_json_size = json_array_get_count(bimap_json);
            for (size_t idx = 0; idx< bimap_json_size; idx++) {
                JSON_Array* element_json = json_array_get_array(bimap_json, idx);
                if (element_json == nullptr) {
                    printf("no bimap array element at idx=%d\r\n", idx);
                    set_default();
                    return false;
                }
                T element[2];
                for (size_t jdx = 0; jdx < 2; jdx++) {
                    JSON_Value* val = json_array_get_value(element_json, jdx);
                    if (val == nullptr) {
                        set_default();
                        return false;
                    }
                    if (json_value_get_type(val) == JSONNumber) {
                        element[jdx] = json_array_get_number(element_json, jdx);
                    }
                    else {
                        printf("bimap array value not a number\r\n");
                        set_default();
                        return false;
                    }
                }
                if (-1 == push_back(element[0], element[1])) {
                    printf("failed to push_back new bimap element\r\n");
                    set_default();
                    return false;
                }
            }
        }
        else {
            printf("Could not parse %s from settings\r\n", name);
            return false;
        }
        return true;
    }
private:
    const char* name;
    T minval;
    T maxval;
    std::vector<std::array<T,2>> bimap;
    T number;
};
}