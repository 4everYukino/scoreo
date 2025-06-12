#include "value_lesser.h"

namespace json_spirit {

bool value_lesser::operator () (const Value& l, const Value& r) const
{
    if (l.type() == r.type()) {
        switch (l.type()) {
        case int_type:
            return l.get_int() < r.get_int();
        case bool_type:
            return l.get_bool() < r.get_bool();
        case str_type:
            return l.get_str() < r.get_str();
        case secure_str_type:
            return l.get_secure_str() < r.get_secure_str();
        case real_type:
            return l.get_real() < r.get_real();
        default:
            return false;
        }
    }

    return l.type() < r.type();
}

} /* namespace json_spirit */
// vim: set et ts=4 sts=4 sw=4:
