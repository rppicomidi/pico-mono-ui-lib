#pragma once
#include <cstdint>
#include "parson.h"
namespace rppicomidi
{
template<typename T>
class Setting_number
{
public:
    Setting_number(const char* name_, T minval_, T maxval_, T def_val_) :
        name{name_}, minval{minval_}, maxval{maxval_}, def_val{def_val_}
    {
        set_default();
    }
    Setting_number()=delete;

    virtual ~Setting_number()=default;
    /**
     * @brief Get the minimum value for this setting
     * 
     * @return T the minimum value for this setting
     */
    virtual T get_min() { return minval; }

    virtual void set_min(T minval_) { minval=minval_; }

    /**
     * @brief Get the maximum value for this setting
     * 
     * @return T the maximum value for this setting
     */
    virtual T get_max() { return maxval; }

    virtual void set_max(T maxval_) { maxval=maxval_; }

    /**
     * @brief Get the default value for this setting
     * 
     * @return T the default value for this setting
     */
    virtual T get_default() {return def_val; }

    /**
     * @brief Set the setting value to default
     */
    virtual void set_default() { number = get_default(); }

    /**
     * @brief Get the setting value
     * 
     * @return T the setting value
     */
    virtual T get() {return number; }

    /**
     * @brief set the setting value
     * 
     * @param val the new value
     * @return true if the value is in range
     * @return false if the value is not in range
     */
    virtual bool set(T val)
    {
        if (val < get_min() || val > get_max())
            return false;
        number = val;
        return true;
    }

    /**
     * @brief add delta to the setting value
     * 
     * @param delta the ammount to increment (or if negative, to decrement) the setting
     */
    virtual T incr(int delta)
    {
        T prev = number; // detect number wrapping
        T test = number+delta;
        if (test > prev && delta < 0)
            test = get_min();
        if (test < prev && delta > 0)
            test = get_max();
        if (test < get_min())
            test = get_min();
        if (test > get_max())
            test = get_max();
        set(test);
        return number;
    }

    /**
     * @brief Get the name of this setting
     * @return const char* the name of this setting
     */
    const char* get_name() {return name; }

    /**
     * @brief add the number name value pair to the JSON root object
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
    void serialize(JSON_Object *root_object) {json_object_set_number(root_object, name, number);}

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
        if (json_object_has_value_of_type(root_object, name, JSONNumber)) {
            double val = json_object_get_number(root_object, name);
            if (!set(static_cast<T>(val))) {
                printf("Val %g out of range for %s\r\n", val, name);
                return false;
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
    T def_val;
    T number;
};
}