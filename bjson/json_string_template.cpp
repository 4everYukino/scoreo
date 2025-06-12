#include "json_string_template.h"

#include "dump.h"

#include <ace/Log_Msg.h>
#include <boost/algorithm/string/replace.hpp>

using json_spirit::Value;
using namespace std;

JSON_String_Template::JSON_String_Template(
    const string& tpl, const string& prefix, const string& suffix)
    : Super(tpl, prefix, suffix)
{
}

JSON_String_Template::~JSON_String_Template()
{
}

JSON_String_Template&
JSON_String_Template::operator () (const string& key, const Value& val)
{
    switch (val.type()) {
    case json_spirit::obj_type:
        Super::operator()(key, JSON_Dump(val).c_str());
        break;
    case json_spirit::array_type:
        Super::operator()(key, JSON_Dump(val).c_str());
        break;
    case json_spirit::str_type:
        Super::operator()(key, val.get_str());
        break;
    case json_spirit::secure_str_type:
        ACE_ERROR((LM_CRITICAL,
                   "String template not support secure string.\n"));
        break;
    case json_spirit::bool_type:
        Super::operator()(key, val.get_bool());
        break;
    case json_spirit::int_type:
        val.is_uint64() ? Super::operator()(key, val.get_uint64()) :
                          Super::operator()(key, val.get_int64());
        break;
    case json_spirit::real_type:
        Super::operator()(key, val.get_real());
        break;
    case json_spirit::null_type:
        Super::operator()(key, "NULL");
        break;
    }

    return *this;
}

// vim: set ts=4 sw=4 sts=4 et:
