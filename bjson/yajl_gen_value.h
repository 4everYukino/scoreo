#ifndef YAJL_GEN_VALUE_H
#define YAJL_GEN_VALUE_H

#include "bjson_export.h"
#include "bjson_value.h"

#include <yajl/yajl_gen.h>

void gen_map(yajl_gen g, const bjson::Value& val);

void gen_array(yajl_gen g, const bjson::Value& val);

void gen_string(yajl_gen g, const bjson::Value& val);

void gen_int(yajl_gen g, const bjson::Value& val);

extern "C" BJSON_EXPORT yajl_gen_status
    yajl_gen_value(yajl_gen g, const bjson::Value& val);

#endif
