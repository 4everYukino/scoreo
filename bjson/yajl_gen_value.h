#ifndef YAJL_GEN_VALUE_H
#define YAJL_GEN_VALUE_H

#include "json_spirit_export.h"
#include "json_spirit_value.h"

#include <yajl_gen.h>

extern "C" JSON_SPIRIT_Export

yajl_gen_status yajl_gen_value(yajl_gen g, const json_spirit::Value& val);

#endif

// vim: set et ts=4 sts=4 sw=4:
