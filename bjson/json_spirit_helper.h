/*!
 * \file json_spirit_helper.h
 * \brief json_spirit helper functions.
 * \author Like Ma <likemartinma@gmail.com>
 */

#ifndef JSON_SPIRIT_HELPER_H
#define JSON_SPIRIT_HELPER_H

#include "json_spirit_export.h"
#include "json_spirit_value.h"

namespace json_spirit {

JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, Value& val);

JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, String_type& val);

JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, secure_string& val);

JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, int& val);

JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, unsigned& val);

#if __SIZEOF_LONG__ == 4
JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, long& val);

JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, unsigned long& val);
#endif // __SIZEOF_LONG__ == 4

JSON_SPIRIT_Export
bool object_get(
    const Object& obj, const String_type& name, int64_t& val);

JSON_SPIRIT_Export
bool object_get(
    const Object& obj, const String_type& name, uint64_t& val);

JSON_SPIRIT_Export
bool object_get_ex(const Object& obj, const String_type& name, bool& val);

JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, bool& val);

JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, Object& val);

JSON_SPIRIT_Export
bool object_get(const Object& obj, const String_type& name, Array& val);

JSON_SPIRIT_Export
bool object_get_time_t(
    const Object& obj, const String_type& name, time_t& val);

template <typename T>
bool object_get(const Object& obj, const String_type& name, T& val)
{
    int64_t int64_val;
    if (!object_get(obj, name, int64_val))
        return false;
    val = static_cast<T>(int64_val);
    return true;
}


JSON_SPIRIT_Export
bool object_get_swap(Object& obj, const String_type& name, Object& val);

JSON_SPIRIT_Export
bool object_get_swap(Object& obj, const String_type& name, Array& val);


JSON_SPIRIT_Export
const Value& object_get_value_or_throw(
    const Object& obj, const String_type& name);

JSON_SPIRIT_Export
Value& object_get_value_or_throw(
    Object& obj, const String_type& name);

JSON_SPIRIT_Export
const String_type& object_get_str_or_throw(
    const Object& obj, const String_type& name);

JSON_SPIRIT_Export
const String_type& object_get_str_or_throw(
    const Object& obj, const String_type& name, const String_type& default_);

JSON_SPIRIT_Export
String_type& object_get_str_or_throw(
    Object& obj, const String_type& name);

JSON_SPIRIT_Export
String_type& object_get_str_or_throw(
    Object& obj, const String_type& name, String_type& default_);

JSON_SPIRIT_Export
const secure_string& object_get_secure_str_or_throw(
    const Object& obj, const String_type& name);

JSON_SPIRIT_Export
const secure_string& object_get_secure_str_or_throw(
    const Object& obj, const String_type& name, const secure_string& default_);

JSON_SPIRIT_Export
secure_string& object_get_secure_str_or_throw(
    Object& obj, const String_type& name);

JSON_SPIRIT_Export
secure_string& object_get_secure_str_or_throw(
    Object& obj, const String_type& name, secure_string& default_);

JSON_SPIRIT_Export
int object_get_int_or_throw(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
unsigned object_get_uint_or_throw(const Object& obj, const String_type& name);

#if __SIZEOF_LONG__ == 4
JSON_SPIRIT_Export
long object_get_long_or_throw(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
unsigned long object_get_ulong_or_throw(const Object& obj, const String_type& name);
#endif // __SIZEOF_LONG__ == 4

JSON_SPIRIT_Export
int64_t object_get_int64_or_throw(
    const Object& obj, const String_type& name);

JSON_SPIRIT_Export
uint64_t object_get_uint64_or_throw(
    const Object& obj, const String_type& name);

JSON_SPIRIT_Export
const Array& object_get_array_or_throw(
    const Object& obj, const String_type& name);

JSON_SPIRIT_Export
const Array& object_get_array_or_throw(
    const Object& obj, const String_type& name, const Array& default_);

JSON_SPIRIT_Export
Array& object_get_array_or_throw(
    Object& obj, const String_type& name);

JSON_SPIRIT_Export
Array& object_get_array_or_throw(
    Object& obj, const String_type& name, Array& default_);

JSON_SPIRIT_Export
const Object& object_get_object_or_throw(
    const Object& obj, const String_type& name);

JSON_SPIRIT_Export
const Object& object_get_object_or_throw(
    const Object& obj, const String_type& name, const Object& default_);

JSON_SPIRIT_Export
Object& object_get_object_or_throw(
    Object& obj, const String_type& name);

JSON_SPIRIT_Export
Object& object_get_object_or_throw(
    Object& obj, const String_type& name, Object& default_);

JSON_SPIRIT_Export
bool object_get_bool_or_throw(
    const Object& obj, const String_type& name);


JSON_SPIRIT_Export
const Value* object_get_value(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
Value* object_get_value(Object& obj, const String_type& name);

JSON_SPIRIT_Export
String_type* object_get_str(Object& obj, const String_type& name);

JSON_SPIRIT_Export
const String_type* object_get_str(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
const char* object_get_cstr(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
secure_string* object_get_secure_str(Object& obj, const String_type& name);

JSON_SPIRIT_Export
const secure_string* object_get_secure_str(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
const char* object_get_secure_cstr(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
const Object* object_get_object(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
Object* object_get_object(Object& obj, const String_type& name);

JSON_SPIRIT_Export
const Array* object_get_array(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
Array* object_get_array(Object& obj, const String_type& name);

JSON_SPIRIT_Export
void format_str_by_obj(const json_spirit::Object& params, String_type& buf);

JSON_SPIRIT_Export
double object_get_real_or_throw(const Object& obj, const String_type& name);

JSON_SPIRIT_Export
uint64_t object_get_num_or_throw(const Object& obj, const String_type& name);

} // namespace json_spirit

#endif // JSON_SPIRIT_HELPER_H
// vim: set et ts=4 sts=4 sw=4:
