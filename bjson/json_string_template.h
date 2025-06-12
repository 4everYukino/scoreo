/// \file json_string_template.h
/// \brief easy format for json_spirit::Value
/// \author Bin Lan <lanbin.scutech@gmail.com>
/// \date 2017-11-03
#ifndef JSON_STRING_TEMPLATE_H
#define JSON_STRING_TEMPLATE_H

#include "json_spirit_export.h"
#include "json_spirit_value.h"

#include "scrt/string_template.h"

#include <map>
#include <string>
#include <ace/Basic_Types.h>

class JSON_SPIRIT_Export JSON_String_Template: public String_Template
{
public:
    typedef String_Template Super;

    JSON_String_Template(const std::string& tpl,
                         const std::string& prefix = "{{",
                         const std::string& suffix = "}}");
    ~JSON_String_Template();

    JSON_String_Template& operator () (const std::string& key,
                                       const json_spirit::Value& val);
};

#endif /* !JSON_STRING_TEMPLATE_H */
// vim: set ts=4 sw=4 sts=4 et:
