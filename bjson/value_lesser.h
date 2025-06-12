/// \file value_lesser.h
/// \brief json spirit Value less comparser.
/// \author Like Ma <likemartinma@gmail.com>
#ifndef JSON_SPIRIT_VALUE_LESSER_H
#define JSON_SPIRIT_VALUE_LESSER_H

#include "json_spirit_export.h"
#include "json_spirit_value.h"

namespace json_spirit {

struct JSON_SPIRIT_Export value_lesser {
    bool operator () (const Value& l, const Value& r) const;
};

} // namespace json_spirit

#endif // !JSON_SPIRIT_VALUE_LESSER_H
// vim: set ts=4 sw=4 sts=4 et:
