/*!
 * \file number_to_value.h
 * \brief Convert number (integer/real) string to json_spirit::Value
 * \author Like Ma <likemartinma@gmail.com>
 */

#ifndef NUMBER_TO_VALUE_H
#define NUMBER_TO_VALUE_H

#include "json_spirit_export.h"
#include "json_spirit_value.h"

JSON_SPIRIT_Export void
number_to_value(const char* val, size_t len, json_spirit::Value& v);

#endif /* NUMBER_TO_VALUE_H */
// vim: set et ts=4 sts=4 sw=4:
