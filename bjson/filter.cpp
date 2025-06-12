#include "filter.h"

namespace json_spirit {

#define FILTER_HELPER_DEFINE(TYPE) \
const FILTER_HELPER_TYPE(TYPE) get_##TYPE = \
    make_pred_conv_pair(Type_Checker{TYPE##_type}, get_##TYPE##_helper);

FILTER_HELPER_DEFINE(obj)
FILTER_HELPER_DEFINE(array)
FILTER_HELPER_DEFINE(str)
FILTER_HELPER_DEFINE(secure_str)
FILTER_HELPER_DEFINE(bool)
FILTER_HELPER_DEFINE(real)

#define FILTER_INT_HELPER_DEFINE(TYPE) \
const FILTER_HELPER_TYPE(TYPE) get_##TYPE = \
    make_pred_conv_pair(Type_Checker{int_type}, get_##TYPE##_helper);

FILTER_INT_HELPER_DEFINE(int)
FILTER_INT_HELPER_DEFINE(uint)
FILTER_INT_HELPER_DEFINE(int64)
FILTER_INT_HELPER_DEFINE(uint64)

}   // namespace json_spirit

// vim: set ts=4 sw=4 sts=4 et:
