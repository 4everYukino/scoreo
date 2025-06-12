#include "update.h"

#include "scrt/check_macros.h"
#include <cstdarg>

using json_spirit::Value;
using json_spirit::Object;

void update_json(const Value& src, Value& tgt, unsigned flags)
{
    if (src.type() != json_spirit::obj_type ||
            tgt.type() != json_spirit::obj_type) {
        tgt = src;
        return;
    }

    update_json(src.get_obj(), tgt.get_obj(), flags);
}

void update_json(const Object& src, Object& tgt, unsigned flags)
{
    for (const auto& i : src) {
        if (flags & json_spirit::FG_ERASE_IF_NULL && i.second == Value::null)
            tgt.erase(i.first);
        else
            update_json(i.second, tgt[i.first], flags);
    }
}

void update_json_partial(const Object* src, Object* tgt, ...)
{
    CHECK_PTR(src);
    CHECK_PTR(tgt);

    va_list l;
    va_start(l, tgt);

    const char* v = NULL;
    Object::const_iterator src_it;

    while ((v = va_arg(l, const char*))) {
        src_it = src->find(v);
        if (src_it == src->end())
            continue;

        update_json(src_it->second, (*tgt)[v], 0);
    }

    va_end(l);
}

// vim: set ts=4 sw=4 sts=4 et:
