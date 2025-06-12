#include "yajl_gen_value.h"
#include <ace/Basic_Types.h>
#include <ace/OS_NS_stdio.h>

using namespace json_spirit;
using namespace std;

yajl_gen_status yajl_gen_value(yajl_gen g, const Value& val)
{
    switch (val.type()) {
    case obj_type:
        yajl_gen_map_open(g);
        {
            const Object& obj = val.get_obj();
            for (Object::const_iterator i = obj.begin(); i != obj.end(); ++i) {
                yajl_gen_string(g, (const unsigned char*)i->first.c_str(),
                                i->first.length());
                yajl_gen_value(g, i->second);
            }
        }
        yajl_gen_map_close(g);
        break;
    case array_type:
        yajl_gen_array_open(g);
        {
            const Array& arr = val.get_array();
            for (Array::size_type i = 0; i < arr.size(); ++i) {
                yajl_gen_value(g, arr[i]);
            }
        }
        yajl_gen_array_close(g);
        break;
    case str_type:
        {
            const string& str = val.get_str();
            yajl_gen_string(g, (const unsigned char*)str.c_str(),
                            str.length());
        }
        break;
    case secure_str_type:
        {
            const secure_string& str = val.get_secure_str();
            yajl_gen_string(g, (const unsigned char*)str.c_str(),
                            str.length());
        }
        break;
    case bool_type:
        yajl_gen_bool(g, val.get_bool());
        break;
    case int_type:
        {
            char buf[32];
            int len = val.is_uint64() ?
                ACE_OS::snprintf(buf, sizeof(buf),
                                 ACE_UINT64_FORMAT_SPECIFIER_ASCII,
                                 val.get_uint64()) :
                ACE_OS::snprintf(buf, sizeof(buf),
                                 ACE_INT64_FORMAT_SPECIFIER_ASCII,
                                 val.get_int64());
            yajl_gen_number(g, buf, len);
        }
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

// vim: set et ts=4 sts=4 sw=4:
