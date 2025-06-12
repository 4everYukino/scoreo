#include "duplicate.h"
#include "assign.h"
#include "extract.h"
#include "scrt/check_macros.h"

using namespace json_spirit;
using namespace std;

JSON_Duplicator::JSON_Duplicator(Object val)
    : val_(move(val))
{
}

DEFAULT_DTOR_DEFINE(JSON_Duplicator);

#if __cpp_ref_qualifiers >= 200710
Object JSON_Duplicator::to_object(const initializer_list<const char*>& keys) &
{
    Object result;

    for (const auto key : keys) {
        CHECK_PTR_RETURN(key, result);
        const auto it = val_.find(key);
        if (it != val_.end())
            result[key] = it->second;
    }

    return result;
}

Object JSON_Duplicator::to_object(const initializer_list<const char*>& keys) &&
{
    Object result;

    for (const auto key : keys) {
        CHECK_PTR_RETURN(key, result);
        const auto it = val_.find(key);
        if (it != val_.end())
            result[key] = move(it->second);
    }

    return result;
}

#else // __cpp_ref_qualifiers < 200710
Object JSON_Duplicator::to_object(const initializer_list<const char*>& keys)
{
    Object result;

    for (const auto key : keys) {
        CHECK_PTR_RETURN(key, result);
        const auto it = val_.find(key);
        if (it != val_.end())
            result[key] = it->second;
    }

    return result;
}
#endif // __cpp_ref_qualifiers < 200710
// vim: set ts=4 sw=4 sts=4 et:
