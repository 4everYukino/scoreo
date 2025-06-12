#include "yajl_gen_value.h"

#include <ace/Basic_Types.h>
#include <ace/OS_NS_stdio.h>

using namespace bjson;

using namespace std;

void gen_map(yajl_gen g, const Value& val)
{
    yajl_gen_map_open(g);

    const Object& obj = val.get_obj();
    for (const auto& i : obj) {
        yajl_gen_string(g, (const unsigned char*)i->first.c_str(), i->first.length());
        yajl_gen_value(g, i->second);
    }

    yajl_gen_map_close(g);
}

void gen_array(yajl_gen g, const Value& val)
{
    yajl_gen_array_open(g);

    const Array& arr = val.get_array();
    for (const auto& i : arr) {
        yajl_gen_value(g, i);
    }

    yajl_gen_array_close(g);
}

void gen_string(yajl_gen g, const Value& val)
{
    const string& str = val.get_str();
    yajl_gen_string(g,
                    (const unsigned char*)str.c_str(),
                    str.length());
}

void gen_int(yajl_gen g, const Value& val)
{
    char buf[32];
    int len = val.is_uint64() ? ACE_OS::snprintf(buf, sizeof(buf), "%lu", val.get_uint64())
                              : ACE_OS::snprintf(buf, sizeof(buf), "%ld", val.get_int64());

    yajl_gen_number(g, buf, len);
}

yajl_gen_status yajl_gen_value(yajl_gen g, const Value& val)
{
    switch (val.type()) {
    case obj_type:
        gen_map(g, val);
        break;
    case array_type:
        gen_array(g, val);
        break;
    case str_type:
        gen_string(g, val);
        break;
    case bool_type:
        yajl_gen_bool(g, val.get_bool());
        break;
    case int_type:
        gen_int(g, val);
        break;
    case real_type:
        yajl_gen_double(g, val.get_real());
        break;
    case null_type:
        yajl_gen_null(g);
        break;
    }

    return yajl_gen_status_ok;
}
