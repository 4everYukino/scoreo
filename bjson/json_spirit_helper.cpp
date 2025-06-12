#include "json_spirit_helper.h"

#include "dump.h"

#include "scrt/http_helpers.h"
#include "scrt/time_helper.h"
#include "scrt/string_template.h"

#include <ace/Log_Msg.h>
#include <boost/algorithm/string/replace.hpp>

using namespace std;

namespace json_spirit {

bool object_get(const Object& obj, const String_type& name, Value& val)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        return false;

    val = i->second;
    return true;
}

bool object_get(const Object& obj, const String_type& name, String_type& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != str_type)
        return false;

    val = i->second.get_str();
    return true;
}

bool object_get(const Object& obj, const String_type& name, secure_string& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != secure_str_type)
        return false;

    val = i->second.get_secure_str();
    return true;
}

bool object_get(const Object& obj, const String_type& name, int& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != int_type)
        return false;

    val = i->second.get_int();
    return true;
}

bool object_get(const Object& obj, const String_type& name, unsigned& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != int_type)
        return false;

    val = i->second.get_uint();
    return true;
}

#if __SIZEOF_LONG__ == 4
bool object_get(const Object& obj, const String_type& name, long& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != int_type)
        return false;

    val = i->second.get_long();
    return true;
}

bool object_get(const Object& obj, const String_type& name, unsigned long& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != int_type)
        return false;

    val = i->second.get_ulong();
    return true;
}
#endif // __SIZEOF_LONG__ == 4

bool object_get(
    const Object& obj, const String_type& name, int64_t& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != int_type)
        return false;

    val = i->second.get_int64();
    return true;
}

bool object_get(
    const Object& obj, const String_type& name, uint64_t& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != int_type)
        return false;

    val = i->second.get_uint64();
    return true;
}

bool object_get_ex(const Object& obj, const String_type& name, bool& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != int_type)
        return false;

    val = (i->second.get_int() != 0);
    return true;
}

bool object_get(const Object& obj, const String_type& name, bool& val)
{
    if (object_get_ex(obj, name, val))
        return true;

    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != bool_type)
        return false;

    val = i->second.get_bool();
    return true;
}

bool object_get(const Object& obj, const String_type& name, Object& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != obj_type)
        return false;

    val = i->second.get_obj();
    return true;
}

bool object_get(const Object& obj, const String_type& name, Array& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != array_type)
        return false;

    val = i->second.get_array();
    return true;
}

bool object_get_time_t(const Object& obj, const String_type& name, time_t& val)
{
    const auto i = obj.find(name);
    return i == obj.end() || i->second.type() != str_type
                   ? false
                   : iso8601_to_posix_time(
                            i->second.get_str().c_str(), val);
}

bool object_get_swap(Object& obj, const String_type& name, Object& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != obj_type)
        return false;

    val.swap(i->second.get_obj());
    return true;
}

bool object_get_swap(Object& obj, const String_type& name, Array& val)
{
    const auto i = obj.find(name);
    if (i == obj.end() || i->second.type() != array_type)
        return false;

    val.swap(i->second.get_array());
    return true;
}

const Value& object_get_value_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    return i->second;
}

Value& object_get_value_or_throw(
    Object& obj, const String_type& name)
{
    auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    return i->second;
}

const String_type& object_get_str_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != str_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not string type in JSON object!"));

    return i->second.get_str();
}

const String_type& object_get_str_or_throw(
    const Object& obj, const String_type& name, const String_type& default_)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        return default_;

    if (i->second.type() != str_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not string type in JSON object!"));

    return i->second.get_str();
}

String_type& object_get_str_or_throw(
    Object& obj, const String_type& name)
{
    auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != str_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not string type in JSON object!"));

    return i->second.get_str();
}

String_type& object_get_str_or_throw(
    Object& obj, const String_type& name, String_type& default_)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        return default_;

    if (i->second.type() != str_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not string type in JSON object!"));

    return i->second.get_str();
}

const secure_string& object_get_secure_str_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != secure_str_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not secure string type in JSON object!"));

    return i->second.get_secure_str();
}

const secure_string& object_get_secure_str_or_throw(
    const Object& obj, const String_type& name, const secure_string& default_)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        return default_;

    if (i->second.type() != secure_str_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not secure string type in JSON object!"));

    return i->second.get_secure_str();
}

secure_string& object_get_secure_str_or_throw(
    Object& obj, const String_type& name)
{
    auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != secure_str_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not secure string type in JSON object!"));

    return i->second.get_secure_str();
}

secure_string& object_get_secure_str_or_throw(
    Object& obj, const String_type& name, secure_string& default_)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        return default_;

    if (i->second.type() != secure_str_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not secure string type in JSON object!"));

    return i->second.get_secure_str();
}

int object_get_int_or_throw(const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != int_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not integer type in JSON object!"));

    return i->second.get_int();
}

unsigned object_get_uint_or_throw(const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != int_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not integer type in JSON object!"));

    return i->second.get_uint();
}

#if __SIZEOF_LONG__ == 4
long object_get_long_or_throw(const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != int_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not integer type in JSON object!"));

    return i->second.get_long();
}

unsigned long object_get_ulong_or_throw(const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != int_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not integer type in JSON object!"));

    return i->second.get_ulong();
}
#endif // __SIZEOF_LONG__ == 4

int64_t object_get_int64_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != int_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not integer type in JSON object!"));

    return i->second.get_int64();
}

uint64_t object_get_uint64_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != int_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not integer type in JSON object!"));

    return i->second.get_uint64();
}

const Array& object_get_array_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != array_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not array type in JSON object!"));

    return i->second.get_array();
}

const Array& object_get_array_or_throw(
    const Object& obj, const String_type& name, const Array& default_)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        return default_;

    if (i->second.type() != array_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not array type in JSON object!"));

    return i->second.get_array();
}

Array& object_get_array_or_throw(
    Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != array_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not array type in JSON object!"));

    return i->second.get_array();
}

Array& object_get_array_or_throw(
    Object& obj, const String_type& name, Array& default_)
{
    auto i = obj.find(name);
    if (i == obj.end())
        return default_;

    if (i->second.type() != array_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not array type in JSON object!"));

    return i->second.get_array();
}

const Object& object_get_object_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != obj_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not object type in JSON object!"));

    return i->second.get_obj();
}

const Object& object_get_object_or_throw(
    const Object& obj, const String_type& name, const Object& default_)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        return default_;

    if (i->second.type() != obj_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not object type in JSON object!"));

    return i->second.get_obj();
}

Object& object_get_object_or_throw(Object& obj, const String_type& name)
{
    const Object::iterator i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != obj_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not object type in JSON object!"));

    return i->second.get_obj();
}

Object& object_get_object_or_throw(
    Object& obj, const String_type& name, Object& default_)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        return default_;

    if (i->second.type() != obj_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not object type in JSON object!"));

    return i->second.get_obj();
}

bool object_get_bool_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != bool_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not bool type in JSON object!"));

    return i->second.get_bool();
}

const Value* object_get_value(const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    return i == obj.end() ? 0 : &i->second;
}

Value* object_get_value(Object& obj, const String_type& name)
{
    return (Value*)object_get_value((const Object&)obj, name);
}

String_type* object_get_str(
    Object& obj, const String_type& name)
{
    auto i = obj.find(name);
    return i == obj.end() || i->second.type() != str_type
                   ? nullptr
                   : &i->second.get_str();
}

const String_type* object_get_str(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    return i == obj.end() || i->second.type() != str_type
                   ? nullptr
                   : &i->second.get_str();
}

const char* object_get_cstr(
    const Object& obj, const String_type& name)
{
    const auto str = object_get_str(obj, name);
    return str ? str->c_str() : 0;
}


secure_string* object_get_secure_str(
    Object& obj, const String_type& name)
{
    auto i = obj.find(name);
    return i == obj.end() || i->second.type() != secure_str_type
                   ? nullptr
                   : &i->second.get_secure_str();
}

const secure_string* object_get_secure_str(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    return i == obj.end() || i->second.type() != secure_str_type
                   ? nullptr
                   : &i->second.get_secure_str();
}

const char* object_get_secure_cstr(
    const Object& obj, const String_type& name)
{
    const auto str = object_get_secure_str(obj, name);
    return str ? str->c_str() : 0;
}

const Object* object_get_object(const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    return i == obj.end() || i->second.type() != obj_type
                  ? nullptr
                  : &i->second.get_obj();
}

Object* object_get_object(Object& obj, const String_type& name)
{
    return (Object*)object_get_object((const Object&)obj, name);
}

const Array* object_get_array(const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    return i == obj.end() || i->second.type() != array_type
                   ? nullptr
                   : &i->second.get_array();
}

Array* object_get_array(Object& obj, const String_type& name)
{
    return (Array*)object_get_array((const Object&)obj, name);
}

void format_str_by_obj(const json_spirit::Object& params, String_type& buf)
{
    String_Template tpl(buf);
    for (const auto& i : params) {
        const string& key = i.first;
        const Value& val = i.second;
        switch (val.type()) {
        case json_spirit::int_type:
            if (val.is_uint64())
                tpl(key, val.get_uint64());
            else
                tpl(key, val.get_int64());

            break;
        case json_spirit::str_type:
            tpl(key, val.get_str());
            break;
        case json_spirit::secure_str_type:
            ACE_ERROR((LM_CRITICAL,
                       "String template not support secure string.\n"));
            break;
        case json_spirit::real_type:
            tpl(key, val.get_real());
            break;
        case json_spirit::null_type:
            tpl(key, "null");
            break;
        case json_spirit::bool_type:
            tpl(key, val.get_bool());
            break;
        default:
            tpl(key, JSON_Dump(val).c_str());
            break;
        }
    }

    tpl.str(buf);
}

double object_get_real_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    if (i->second.type() != real_type)
        throw domain_error(string("Attribute '") + name
            + string("' is not real type in JSON object!"));

    return i->second.get_real();
}

uint64_t object_get_num_or_throw(
    const Object& obj, const String_type& name)
{
    const auto i = obj.find(name);
    if (i == obj.end())
        throw domain_error(string("No attribute '") + name
            + string("' in JSON object!"));

    switch (i->second.type()) {
    case int_type:
        return i->second.get_uint64();
    case real_type:
        return i->second.get_real();
    default:
        throw domain_error(string("Attribute '") + name
            + string("' is not int64 or real type in JSON object!"));
    }
}

} /* namespace json_spirit */
// vim: set et ts=4 sts=4 sw=4:
