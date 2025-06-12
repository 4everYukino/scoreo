#include "number_to_value.h"
#include "scrt/natoi.h"
#include <ace/OS_NS_stdlib.h>
#include <limits.h>

using namespace json_spirit;

void number_to_value(const char* val, size_t len, Value& v)
{
    char buf[128];
    if (len >= sizeof(buf))
        return;

    strncpy(buf, val, len)[len] = 0;
    if (strcspn(buf, ".eE") == len) {
        if (buf[0] == '-') {
            const auto num = natoi<int64_t>(val, len);
            v = num >= INT_MIN && num <= INT_MAX ? (int)num : num;
        } else {
            v = natoi<uint64_t>(val, len);
        }
    } else {
        v = strtod(buf, 0);
    }
}

// vim: set et ts=4 sts=4 sw=4:
