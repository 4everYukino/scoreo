/// \file update.h
/// \author Bin Lan <lanbin.scutech@gmail.com>
#ifndef JSON_SPIRIT_UPDATE_H
#define JSON_SPIRIT_UPDATE_H

#include "json_spirit_export.h"
#include "json_spirit_value.h"

namespace json_spirit {

enum {
    FG_ERASE_IF_NULL = (1 << 0),
};

} // namespace json_spirit

JSON_SPIRIT_Export void update_json(const json_spirit::Value& src,
                                    json_spirit::Value& tgt,
                                    unsigned flags);

JSON_SPIRIT_Export void update_json(const json_spirit::Object& src,
                                    json_spirit::Object& tgt,
                                    unsigned flags = 0);

// variadic arguments are pointers of const char, and must ends with NULL
JSON_SPIRIT_Export void update_json_partial(const json_spirit::Object* src,
                                            json_spirit::Object* tgt,
                                            ...);

#endif // !JSON_SPIRIT_UPDATE_H
// vim: set ts=4 sw=4 sts=4 et:
