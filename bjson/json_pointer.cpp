#include "json_pointer.h"

#include "json_spirit/dump.h"
#include "json_spirit/json_spirit_helper.h"
#include "scrt/check_macros.h"
#include "scrt/smart_ptr_macros.h"

#include <ace/Log_Msg.h>
#include <cstdlib>
#include <string>

using namespace std;

enum {
    ESCAPE_BOUNDARY_PREFIX_LEN = 2,
    ESCAPE_BOUNDARY_LEN = 4
};

#define ERROR_RETURN(MSG, RET, COND) \
do { \
if (COND) \
    ACE_ERROR(MSG); \
return RET; \
} while (0)

#define STRIP_BOUNDARY_ASSIGN(METHOD) \
{ \
    const auto strip = is_force_obj_type(seg); \
    const auto len = strlen(seg) - (strip ? ESCAPE_BOUNDARY_LEN : 0); \
    string key(strip ? seg + ESCAPE_BOUNDARY_PREFIX_LEN : seg, len); \
    *curr = &METHOD(parent->get_obj()[move(key)]); \
}

static bool is_force_obj_type(const char* str)
{
    // Integer literals surrounded by '~2' means that create an
    // element value which it's type is object, and it's key
    // is the integer literals e.g.
    // JSON_Pointer('/path/to/~2123~2').set(doc, "foo")
    // create follows json
    // { "path" : { "to": { "123": "foo" } } }

    if (!str || *str == '\0')
        return false;

    size_t len = strlen(str);
    return len > ESCAPE_BOUNDARY_LEN && str[0] == '~' && str[1] == '2' &&
               str[len - 2] == '~' && str[len - 1] == '2';
}

static const char* unescape(
        const char* const begin,
        const char* const end,
        char* const target)
{
    if (!begin || !end || begin == end) {
        *target = '\0';
        return target;
    }

    const char* p = begin, * q = end;
    char* j = target;

    while (p + 1 < q) {
        if (*p == '~' && *(p + 1) == '1') {
            *j++ = '/';
            p += 2;
        } else if (*p == '~' && *(p + 1) == '0') {
            *j++ = '~';
            p += 2;
        } else {
            *j++ = *p++;
        }
    }

    if (p < q)
        *j++ = *p;

    *j = '\0';
    return target;
}

static bool parse_integer(
        const char* seg,
        const char* path,
        int& idx,
        bool log)
{
    CHECK_PTR_RETURN(seg, false);

    if (*seg == '0' && *(seg + 1) != '\0')
        ERROR_RETURN((LM_ERROR,
                      "Failed to check json pointer '%s', "
                      "unexpected prefix '0' in array segment '%s'.\n",
                      path,
                      seg),
                      false,
                      log);

    char* p;
    idx = (int)strtoul(seg, &p, 10);
    if (*p != '\0')
        ERROR_RETURN((LM_ERROR,
                      "Failed to check json pointer '%s', "
                      "unexpected digital array index "
                      "in segment '%s'.\n",
                      path,
                      seg),
                      false,
                      log);

    return true;
}

namespace json_spirit {

inline Value& be_object(Value &val)
{
    if (val.type() != obj_type)
        val = Object();

    return val;
}

inline Value& be_array(Value& val)
{
    if (val.type() != array_type)
        val = Array();

    return val;
}

inline bool is_array_segment(const char* seg)
{
    return *seg != '\0'
               && (strcmp(seg, "-") == 0
               || *(seg + strspn(seg, "0123456789")) == '\0');
}

JSON_Pointer::JSON_Pointer(const char* val, bool log, bool copy)
    : log_(log)
{
    if (val)
        path(val, copy);
}

void JSON_Pointer::path(const char* val, bool copy)
{
    CHECK_PTR(val);

    size_t len = strlen(val) + 1;
    if (!path_ || len > strlen(path_) + 1)
        buf_ = make_unique<char[]>(len);

    if (copy) {
        path_buf_.assign(val);
        path_ = path_buf_.c_str();
    } else {
        path_ = val;
    }
}

const char* JSON_Pointer::path() const
{
    return path_;
}

template <typename T>
bool JSON_Pointer::assign_value(Value* head, string& key, T&& val) const
{
    CHECK_PTR_RETURN(head, false);

    if (is_force_obj_type(key.c_str()))
        key.assign(key.c_str() + 2, key.size() - 4);

    switch (head->type()) {
    case obj_type:
        head->get_obj()[key] = std::forward<T>(val);
        break;
    case array_type:
        {
            auto& arr = head->get_array();
            if (key == "-") {
                arr.push_back(std::forward<T>(val));
            } else {
                int idx = 0;
                if (!parse_integer(key.c_str(), path_, idx, log_))
                    return false;

                if ((size_t)idx < arr.size()) {
                    arr[idx] = std::forward<T>(val);
                } else if ((size_t)idx == arr.size()) {
                    arr.push_back(std::forward<T>(val));
                } else {
                    ERROR_RETURN((LM_ERROR,
                                  "Failed to execute json pointer '%s', "
                                  "array index '%s' overflow, "
                                  "current array size %d.\n",
                                  path_,
                                  key.c_str(),
                                  arr.size()),
                                  false,
                                  log_);
                }
            }
        }

        break;
    default:
        ERROR_RETURN((LM_ERROR,
                      "Unexpected error occurred "
                      "while execute json pointer '%s', "
                      "try to assign value on literal "
                      "json data type.\n",
                      path_),
                      false,
                      log_);
    }

    return true;
}

bool JSON_Pointer::locate(Value** curr, string& seg) const
{
    CHECK_PTR_RETURN(curr, false);

    if (*path_ != '/')
        ERROR_RETURN((LM_ERROR,
                      "Illegally formed json pointer syntax '%s', "
                      "path should start with slash '/'!\n",
                      path_),
                      false,
                      log_);

    if (*(path_ + 1) == '\0') {
        be_object(**curr);
        return true;
    }

    const char* p = path_ + 1, * q, * end = path_ + strlen(path_);
    char* next_seg = buf_.get();
    bool first = true;

    while (p < end && (q = strchr(p, '/'))) {
        unescape(p, q, next_seg);
        p = q + 1;

        if (first) {
            if (!assign_root(*curr, next_seg))
                return false;

            seg.assign(next_seg);
            first = false;
            continue;
        }

        if (!assign_segment(curr, seg.c_str(), next_seg))
            return false;

        seg.assign(next_seg);
    }

    if (first)
        seg.assign(path_ + 1);

    if (p < end) {
        bool ret = first ? assign_root(*curr, unescape(p, end, next_seg))
                         : assign_segment(curr,
                                          seg.c_str(),
                                          unescape(p, end, next_seg));

        if (!ret)
            return false;

        seg.assign(next_seg);
    } else if (p == end && *(end - 1) == '/') {
        bool ret = first ? assign_root(*curr, "")
                         : assign_segment(curr, seg.c_str(), "");

        if (!ret)
            return false;

        seg.assign("");
    }

    return *curr != nullptr;
}

bool JSON_Pointer::assign_segment(
        Value** curr,
        const char* seg,
        const char* next_seg) const
{
    CHECK_PTR_RETURN(curr, false);

    is_force_obj_type(next_seg) || !is_array_segment(next_seg)
        ? assign_object(curr, seg)
        : assign_array(curr, seg);
    return true;
}

bool JSON_Pointer::assign_root(Value* root, const char* seg)
{
    CHECK_PTR_RETURN(root, false);

    is_force_obj_type(seg) || !is_array_segment(seg)
        ? be_object(*root)
        : be_array(*root);
    return true;
}

bool JSON_Pointer::assign_array(Value** curr, const char* seg) const
{
    CHECK_PTR_RETURN(curr, false);

    Value* parent = *curr;
    switch (parent->type()) {
    case obj_type:
        STRIP_BOUNDARY_ASSIGN(be_array);
        break;
    case array_type:
        {
            auto& arr = parent->get_array();
            int idx = 0;
            if (*(seg + 1) == '\0' && *seg == '-')
                idx = static_cast<int>(arr.size());
            else if (!parse_integer(seg, path_, idx, log_))
                return false;

            if ((size_t)idx > arr.size())
                ERROR_RETURN((LM_ERROR,
                              "Unexpected error occurred while "
                              "assign pointer '%d', "
                              "array index '%d' overflowed!\n",
                              idx),
                              false,
                              log_);

            if ((size_t)idx == arr.size()) {
                *curr = &(*arr.insert(arr.end(), Array()));
            } else {
                *curr = &be_array(arr[idx]);
            }
        }

        break;
    default:
        ERROR_RETURN((LM_ERROR,
                      "Unexpected error occurred while "
                      "assign json pointer '%s' segment '%s', "
                      "ancestor value type is base element.\n",
                      path_,
                      seg),
                      false,
                      log_);
    }

    return true;
}

bool JSON_Pointer::assign_object(Value** curr, const char* seg) const
{
    CHECK_PTR_RETURN(curr, false);

    Value* parent = *curr;
    switch (parent->type()) {
    case obj_type:
        STRIP_BOUNDARY_ASSIGN(be_object);
        break;
    case array_type:
        {
            auto& arr = parent->get_array();
            int idx = 0;
            if (*(seg + 1) == '\0' && *seg == '-')
                idx = static_cast<int>(arr.size());
            else if (!parse_integer(seg, path_, idx, log_))
                return false;

            if ((size_t)idx > arr.size())
                ERROR_RETURN((LM_ERROR,
                              "Unexpected error occurred while "
                              "assign pointer '%d', "
                              "array index '%d' overflowed!\n",
                              idx),
                              false,
                              log_);

            if ((size_t)idx == arr.size()) {
                *curr = &(*arr.insert(arr.end(), Object()));
            } else {
                *curr = &be_object(arr[idx]);
            }
        }

        break;
    default:
        ERROR_RETURN((LM_ERROR,
                      "Unexpected error occurred while "
                      "assign json pointer '%s' segment '%s', "
                      "ancestor value type is base element.\n",
                      path_,
                      seg),
                      false,
                      log_);

    }

    return true;
}

template<typename T>
JSON_Pointer::const_value_wrapper_t<T> JSON_Pointer::retrieve(T& root) const
{
    // TODO Boost the extract processes by using Tire Tree
    // likely data structure to cache path segments.

    if (!buf_.get() || !path_)
        return nullptr;

    // the path form "" is allowed in json pointer, it is used to retrieve
    // the whole documentation.
    //
    // TODO improiving the compiling failure message casing by mismatching
    // retrun type there, Expected const Value*, Get const Object*.
    if (*path_ == '\0')
        return get_fake_root(root);

    if (*path_ != '/')
        ERROR_RETURN((LM_ERROR,
                      "Illegally formed json pointer syntax '%s'.\n",
                      path_),
                      nullptr,
                      log_);

    const size_t len = strlen(path_);
    char* const seg = buf_.get();
    const char* p = path_ + 1, * q, * end = path_ + len;

    auto curr = get_root(root, p, q, end, seg);
    do {
        if (!curr)
            break;

        while (p < end && (q = strchr(p, '/'))) {
            if (!(curr = step_into(curr, unescape(p, q, seg))))
                break;

            p = q + 1;
        }

        if (p < end) {
            curr = step_into(curr, unescape(p, end, seg));
        } else if (p == end && *(end - 1) == '/') {
            curr = step_into(curr, "");
        } /* else {
            should never happen
        } */

    } while (false);

    if (!curr)
        ERROR_RETURN((LM_ERROR,
                      "Failed to execute json pointer '%s', "
                      "json segment '%s' not exist!\n",
                      path_,
                      seg),
                      nullptr,
                      log_);

    return curr;
}

template <typename T, JSON_Pointer::enable_if_is_value_t<T>>
JSON_Pointer::const_value_wrapper_t<T> JSON_Pointer::get_fake_root(T& root) const
{
    return &root;
}

template <typename T, JSON_Pointer::enable_if_is_object_t<T>>
JSON_Pointer::const_value_wrapper_t<T> JSON_Pointer::get_fake_root(T&) const
{
    ERROR_RETURN((LM_ERROR,
                  "Failed to find out an attribute with JSON pointer path "
                  "'%s', unable to treat the root path '%s' as a Value "
                  "when the root object type is Object.\n",
                  path_,
                  path_),
                  nullptr,
                  log_);
}

template <typename T, JSON_Pointer::enable_if_is_array_t<T>>
JSON_Pointer::const_value_wrapper_t<T> JSON_Pointer::get_fake_root(T&) const
{
    ERROR_RETURN((LM_ERROR,
                  "Failed to find out an JSON attribute with JSON pointer path "
                  "'%s', unable to treat the root path '%s' as a Value "
                  "when the root array type is Array.\n",
                  path_,
                  path_),
                  nullptr,
                  log_);
}

template <typename T, JSON_Pointer::enable_if_is_object_t<T>>
JSON_Pointer::const_value_wrapper_t<T> JSON_Pointer::get_root(
        T& root,
        const char* & p,
        const char* & q,
        const char* const end,
        char* const seg) const
{
    if (p >= end)
        return nullptr;

    q = strchr(p, '/');
    if (!q)
        q = end;

    unescape(p, q, seg);
    p = q == end ? end : q + 1;
    return object_get_value(root, seg);
}

template <typename T, JSON_Pointer::enable_if_is_value_t<T>>
JSON_Pointer::const_value_wrapper_t<T> JSON_Pointer::get_root(
        T& root,
        const char* &,
        const char* &,
        const char*,
        char*) const
{
    return &root;
}

template <typename T, JSON_Pointer::enable_if_is_array_t<T>>
JSON_Pointer::const_value_wrapper_t<T> JSON_Pointer::get_root(
        T& root,
        const char* & p,
        const char* & q,
        const char* end,
        char* seg) const
{
    if (p >= end)
        return nullptr;

    q = strchr(p, '/');
    if (!q)
        q = end;

    unescape(p, q, seg);
    p = q == end ? end : q + 1;
    return get_array(root, seg);
}

template <typename T>
T* JSON_Pointer::step_into(T* root, const char* seg) const
{
    static_assert(is_same<typename decay<T>::type, Value>::value,
                  "JSON_Pointer::step_into: T must be Value");

    if (!seg)
        return nullptr;

    if (!root)
        ERROR_RETURN((LM_ERROR,
                      "Unexpected error in executing "
                      "json pointer '%s', segment '%s', "
                      "root is nullptr.\n",
                      path_,
                      seg),
                      nullptr,
                      log_);

    if (*seg == '\0') {
        if (root->type() != obj_type)
            ERROR_RETURN((LM_ERROR,
                          "Unexpected error in execute "
                          "json pointer '%s', segment '%s', "
                          "'/path/to/current/ ' can only be "
                          "used in object type.\n",
                          path_,
                          seg),
                          nullptr,
                          log_);

        return get_object(root->get_obj(), "");
    }

    switch (root->type()) {
    case obj_type:
        return get_object(root->get_obj(), seg);
    case array_type:
        return get_array(root->get_array(), seg);
    default:
        break;
    }

    ERROR_RETURN((LM_ERROR,
                  "Unexpected error in execute "
                  "json pointer '%s', segment '%s', "
                  "current root element '%s' is "
                  "not array or object.\n",
                  path_,
                  seg,
                  JSON_Dump(*root).c_str()),
                  nullptr,
                  log_);
}

const Value* JSON_Pointer::get_object(
        const Object& root,
        const char* name)
{
    if (!name)
        return nullptr;

    return object_get_value(root, name);
}

Value * JSON_Pointer::get_object(
        Object& root,
        const char* name)
{
    if (!name)
        return nullptr;

    return object_get_value(root, name);
}

const Value* JSON_Pointer::get_array(const Array& root, const char* seg) const
{
    int idx = 0;
    if (!parse_integer(seg, path_, idx, log_))
        return nullptr;

    if ((size_t)idx >= root.size())
        ERROR_RETURN((LM_ERROR,
                      "Failed to execute json pointer '%s', "
                      "array index '%s' overflow.\n",
                      path_,
                      seg),
                      nullptr,
                      log_);

    return &root[idx];
}

Value * JSON_Pointer::get_array(Array& root, const char* seg) const
{
    int idx = 0;
    if (!parse_integer(seg, path_, idx, log_))
        return nullptr;

    if ((size_t)idx >= root.size())
        ERROR_RETURN((LM_ERROR,
                      "Failed to execute json pointer '%s', "
                      "array index '%s' overflow.\n",
                      path_,
                      seg),
                      nullptr,
                      log_);

    return &root[idx];
}

#define JSON_POINTER_TAKE_EMPTY_T(TYPE, VALUE) VALUE

#define JSON_POINTER_MOVE(TARGET, VALUE) \
    std::move( (VALUE) )

#define JSON_POINTER_SET_DEFINE_I(TYPE, OPERATOR) \
bool JSON_Pointer::set(Value& doc, TYPE val) const \
{ \
    CHECK_PTR_RETURN(path_, false); \
    auto curr = &doc; \
    string key; \
 \
    return locate(&curr, key) && \
               assign_value(curr, key, OPERATOR(TYPE, val)); \
}

#define JSON_POINTER_SET_DEFINE(TYPE) \
    JSON_POINTER_SET_DEFINE_I(TYPE, JSON_POINTER_TAKE_EMPTY_T) \

JSON_POINTER_SET_DEFINE(bool)
JSON_POINTER_SET_DEFINE(double)
JSON_POINTER_SET_DEFINE(const char*)
JSON_POINTER_SET_DEFINE(int64_t)
JSON_POINTER_SET_DEFINE(uint64_t)
JSON_POINTER_SET_DEFINE(int)
JSON_POINTER_SET_DEFINE(unsigned)

#if __SIZEOF_LONG__ == 4
JSON_POINTER_SET_DEFINE(long)
JSON_POINTER_SET_DEFINE(unsigned long)
#endif // __SIZEOF_LONG__ == 4

#define JSON_POINTER_MOVEABLE_SET_DEFINE(TYPE) \
        JSON_POINTER_SET_DEFINE(const TYPE&) \
        JSON_POINTER_SET_DEFINE_I(TYPE&&, JSON_POINTER_MOVE)

JSON_POINTER_MOVEABLE_SET_DEFINE(Object)
JSON_POINTER_MOVEABLE_SET_DEFINE(Array)
JSON_POINTER_MOVEABLE_SET_DEFINE(Value)
JSON_POINTER_MOVEABLE_SET_DEFINE(string)
JSON_POINTER_MOVEABLE_SET_DEFINE(secure_string)

#define EMPTY_T

#define JSON_POINTER_TAKE_REF_T(TYPE, VALUE) &VALUE

#define JSON_POINTER_MOVE_CONST_CAST(TARGET, VALUE) \
    std::move( const_cast<TARGET>( (VALUE) ) )

#define JSON_POINTER_GET_TYPE_DEFINE_III(INPUT_TYPE, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC, OPERATOR, INIT) \
bool JSON_Pointer::get(INPUT_TYPE doc, TARGET_TYPE val) const \
{ \
    INIT \
    const auto e = retrieve(doc); \
    if (e && e->type() == JSON_TYPE) { \
        val = OPERATOR(TARGET_TYPE, e->RETRIEVE_FUNC()); \
    } else { \
        ERROR_RETURN((LM_ERROR, \
                      "Invalid json pointer path '%s'.\n", \
                      path_), \
                      false, \
                      log_); \
    } \
 \
    return true; \
}

#define JSON_POINTER_GET_TYPE_DEFINE_II(T1, T2, T3, T4, T5) \
    JSON_POINTER_GET_TYPE_DEFINE_III(T1, T2, T3, T4, T5, EMPTY_T)

#define JSON_POINTER_GET_TYPE_DEFINE_I(T1, T2, T3, T4) \
    JSON_POINTER_GET_TYPE_DEFINE_II(T1, T2, T3, T4, JSON_POINTER_TAKE_EMPTY_T)

#define JSON_POINTER_GET_DEFINE(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_TYPE_DEFINE_I(const Value&,  TARGET_TYPE &, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_TYPE_DEFINE_I(const Object&, TARGET_TYPE &, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_TYPE_DEFINE_I(const Array&,  TARGET_TYPE &, JSON_TYPE, RETRIEVE_FUNC)

#define JSON_POINTER_MOVABLE_GET_DEFINE_I(T1, T2, T3, T4) \
    JSON_POINTER_GET_TYPE_DEFINE_II(T1, T2, T3, T4, JSON_POINTER_MOVE_CONST_CAST)

#define JSON_POINTER_MOVABLE_GET_DEFINE(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_DEFINE(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_MOVABLE_GET_DEFINE_I(Value&&, TARGET_TYPE &, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_MOVABLE_GET_DEFINE_I(Object&&, TARGET_TYPE &, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_MOVABLE_GET_DEFINE_I(Array&&, TARGET_TYPE &, JSON_TYPE, RETRIEVE_FUNC) \

// get(const CompoundType&, TargetType&)
JSON_POINTER_GET_DEFINE(bool,            bool_type,         get_bool)
JSON_POINTER_GET_DEFINE(double,          real_type,         get_real)
JSON_POINTER_GET_DEFINE(int,             int_type,          get_int)
JSON_POINTER_GET_DEFINE(unsigned,        int_type,          get_uint)
JSON_POINTER_GET_DEFINE(int64_t,         int_type,          get_int64)
JSON_POINTER_GET_DEFINE(uint64_t,        int_type,          get_uint64)

#if __SIZEOF_LONG__ == 4
JSON_POINTER_GET_DEFINE(long,            int_type,          get_int64)
JSON_POINTER_GET_DEFINE(unsigned long,   int_type,          get_uint64)
#endif // __SIZEOF_LONG__ == 4

// get(CompoundType&&, TargetType&)
JSON_POINTER_MOVABLE_GET_DEFINE(string,          str_type,          get_str)
JSON_POINTER_MOVABLE_GET_DEFINE(secure_string,   secure_str_type,   get_secure_str)
JSON_POINTER_MOVABLE_GET_DEFINE(Object,          obj_type,          get_obj)
JSON_POINTER_MOVABLE_GET_DEFINE(Array,           array_type,        get_array)

#define JSON_POINTER_GET_VALUE_DEFINE_I(INPUT_TYPE, OPERATOR) \
bool JSON_Pointer::get(INPUT_TYPE doc, Value& val) const \
{ \
    if (const auto e = retrieve(doc)) { \
        val = OPERATOR(Value&, *e); \
        return true; \
    } else { \
        ERROR_RETURN((LM_ERROR, \
                      "Invalid json pointer path '%s'.\n", \
                      path_), \
                      false, \
                      log_); \
    } \
 \
    return false; \
}

#define JSON_POINTER_GET_VALUE_DEFINE(TARGET_TYPE) \
    JSON_POINTER_GET_VALUE_DEFINE_I(const TARGET_TYPE &, JSON_POINTER_TAKE_EMPTY_T)

// get(const CompoundType&, Value&)
JSON_POINTER_GET_VALUE_DEFINE(Value)
JSON_POINTER_GET_VALUE_DEFINE(Object)
JSON_POINTER_GET_VALUE_DEFINE(Array)

#define JSON_POINTER_MOVABLE_GET_VALUE_DEFINE(TARGET_TYPE) \
    JSON_POINTER_GET_VALUE_DEFINE_I(TARGET_TYPE &&, JSON_POINTER_MOVE)

// get(CompoundType&&, Value&)
JSON_POINTER_MOVABLE_GET_VALUE_DEFINE(Value)
JSON_POINTER_MOVABLE_GET_VALUE_DEFINE(Object)
JSON_POINTER_MOVABLE_GET_VALUE_DEFINE(Array)

#define JSON_POINTER_GET_OPT_DEFINE_II(INPUT_TYPE, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC, OPERATOR) \
template <> \
JSON_SPIRIT_Export OPT_NS::optional<TARGET_TYPE> JSON_Pointer::get(INPUT_TYPE doc) const \
{ \
    OPT_NS::optional<TARGET_TYPE> val; \
    const auto e = retrieve(doc); \
    if (e && e->type() == JSON_TYPE) { \
        val = OPERATOR(TARGET_TYPE, e->RETRIEVE_FUNC()); \
    } else { \
        ERROR_RETURN((LM_ERROR, \
                      "Invalid json pointer path '%s'.\n", \
                      path_), \
                      val, \
                      log_); \
    } \
 \
    return val; \
} \
template OPT_NS::optional<TARGET_TYPE> JSON_Pointer::get(INPUT_TYPE doc) const;

#define JSON_POINTER_GET_OPT_DEFINE_I(T1, T2, T3, T4) \
    JSON_POINTER_GET_OPT_DEFINE_II(T1, T2, T3, T4, JSON_POINTER_TAKE_EMPTY_T)

#define JSON_POINTER_GET_OPT_DEFINE(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_OPT_DEFINE_I(const Value&, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_OPT_DEFINE_I(const Object&, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_OPT_DEFINE_I(const Array&, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC)

#define JSON_POINTER_MOVABLE_GET_OPT_DEFINE_I(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_OPT_DEFINE_II(Value&&, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC, JSON_POINTER_MOVE) \
    JSON_POINTER_GET_OPT_DEFINE_II(Object&&, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC, JSON_POINTER_MOVE) \
    JSON_POINTER_GET_OPT_DEFINE_II(Array&&, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC, JSON_POINTER_MOVE)

#define JSON_POINTER_MOVEABLE_GET_OPT_DEFINE(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_OPT_DEFINE(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_MOVABLE_GET_OPT_DEFINE_I(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC)

// template<>
// OPT_NS::optional<ValueType> get<ValueType>(const CompoundType&)
JSON_POINTER_GET_OPT_DEFINE(bool,            bool_type,         get_bool)
JSON_POINTER_GET_OPT_DEFINE(double,          real_type,         get_real)
JSON_POINTER_GET_OPT_DEFINE(int,             int_type,          get_int)
JSON_POINTER_GET_OPT_DEFINE(int64_t,         int_type,          get_int64)
JSON_POINTER_GET_OPT_DEFINE(uint64_t,        int_type,          get_uint64)

// template<>
// OPT_NS::optional<ValueType> get<ValueType>(CompoundType&&)
JSON_POINTER_MOVEABLE_GET_OPT_DEFINE(string,          str_type,          get_str)
JSON_POINTER_MOVEABLE_GET_OPT_DEFINE(secure_string,   secure_str_type,   get_secure_str)
JSON_POINTER_MOVEABLE_GET_OPT_DEFINE(Object,          obj_type,          get_obj)
JSON_POINTER_MOVEABLE_GET_OPT_DEFINE(Array,           array_type,        get_array)

#define JSON_POINTER_MOVEABLE_GET_OPT_VALUE_DEFINE_I(TARGET_TYPE, OPERATOR) \
template<> \
JSON_SPIRIT_Export OPT_NS::optional<Value> JSON_Pointer::get<Value>(TARGET_TYPE doc) const \
{ \
    OPT_NS::optional<Value> val; \
    const auto e = retrieve(doc); \
    if (e) { \
        val = OPERATOR(TARGET_TYPE, *e); \
    } else { \
        ERROR_RETURN((LM_ERROR, \
                      "Invalid json pointer path '%s'.\n", \
                      path_), \
                      val, \
                      log_); \
    } \
 \
    return val; \
} \
template OPT_NS::optional<Value> JSON_Pointer::get<Value>(TARGET_TYPE doc) const;

#define JSON_POINTER_GET_OPT_VALUE_DEFINE(TARGET_TYPE) \
    JSON_POINTER_MOVEABLE_GET_OPT_VALUE_DEFINE_I(TARGET_TYPE, JSON_POINTER_TAKE_EMPTY_T)

// OPT_NS::optional<Value> get(const CompoundType&)
JSON_POINTER_GET_OPT_VALUE_DEFINE(const Value&)
JSON_POINTER_GET_OPT_VALUE_DEFINE(const Object&)
JSON_POINTER_GET_OPT_VALUE_DEFINE(const Array&)

#define JSON_POINTER_MOVEABLE_GET_OPT_VALUE_DEFINE(TARGET_TYPE) \
    JSON_POINTER_MOVEABLE_GET_OPT_VALUE_DEFINE_I(TARGET_TYPE, JSON_POINTER_MOVE)

// OPT_NS::optional<Value> get(CompoundType&&)
JSON_POINTER_MOVEABLE_GET_OPT_VALUE_DEFINE(Value&&)
JSON_POINTER_MOVEABLE_GET_OPT_VALUE_DEFINE(Object&&)
JSON_POINTER_MOVEABLE_GET_OPT_VALUE_DEFINE(Array&&)

#define JSON_POINTER_GET_POINTER_DEFINE_I(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC, OPERATOR) \
    JSON_POINTER_GET_TYPE_DEFINE_III(Value&, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC, OPERATOR, val = nullptr;) \
    JSON_POINTER_GET_TYPE_DEFINE_III(Object&, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC, OPERATOR, val = nullptr;) \
    JSON_POINTER_GET_TYPE_DEFINE_III(Array&, TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC, OPERATOR, val = nullptr;)

#define JSON_POINTER_GET_POINTER_DEFINE(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_POINTER_DEFINE_I(TARGET_TYPE* &, JSON_TYPE, RETRIEVE_FUNC, JSON_POINTER_TAKE_REF_T) \

// get(CompoundType&, TargetType* &)
JSON_POINTER_GET_POINTER_DEFINE(Object,        obj_type,        get_obj)
JSON_POINTER_GET_POINTER_DEFINE(Array,         array_type,      get_array)
JSON_POINTER_GET_POINTER_DEFINE(string,        str_type,        get_str)
JSON_POINTER_GET_POINTER_DEFINE(secure_string, secure_str_type, get_secure_str)

#define JSON_POINTER_GET_POINTER_VALUE_DEFINE(INPUT_TYPE, TARGET_TYPE) \
bool JSON_Pointer::get(INPUT_TYPE doc, TARGET_TYPE val) const \
{ \
    val = nullptr; \
    const auto e = retrieve(doc); \
    if (!e) \
        ERROR_RETURN((LM_ERROR, \
                      "Invalid json pointer path '%s'.\n", \
                      path_), \
                      false, \
                      log_); \
 \
    val = e; \
    return true; \
}

// get(CompoundType&, Value* &)
JSON_POINTER_GET_POINTER_VALUE_DEFINE(Value&,  Value *&)
JSON_POINTER_GET_POINTER_VALUE_DEFINE(Object&, Value *&)
JSON_POINTER_GET_POINTER_VALUE_DEFINE(Array&,  Value *&)

#define JSON_POINTER_GET_CONST_POINTER_DEFINE(TARGET_TYPE, JSON_TYPE, RETRIEVE_FUNC) \
    JSON_POINTER_GET_TYPE_DEFINE_III(const Value&,  const TARGET_TYPE* &, JSON_TYPE, RETRIEVE_FUNC, JSON_POINTER_TAKE_REF_T, val = nullptr;) \
    JSON_POINTER_GET_TYPE_DEFINE_III(const Object&, const TARGET_TYPE* &, JSON_TYPE, RETRIEVE_FUNC, JSON_POINTER_TAKE_REF_T, val = nullptr;) \
    JSON_POINTER_GET_TYPE_DEFINE_III(const Array&,  const TARGET_TYPE* &, JSON_TYPE, RETRIEVE_FUNC, JSON_POINTER_TAKE_REF_T, val = nullptr;)

// get(const CompoundType&, const Target* &)
JSON_POINTER_GET_CONST_POINTER_DEFINE(Object,           obj_type,           get_obj)
JSON_POINTER_GET_CONST_POINTER_DEFINE(Array,            array_type,         get_array)
JSON_POINTER_GET_CONST_POINTER_DEFINE(string,           str_type,           get_str)
JSON_POINTER_GET_CONST_POINTER_DEFINE(secure_string,    secure_str_type,    get_secure_str)

// get(const CompoundType&, const Value* &)
JSON_POINTER_GET_POINTER_VALUE_DEFINE(const Value&,  const Value* &)
JSON_POINTER_GET_POINTER_VALUE_DEFINE(const Object&, const Value* &)
JSON_POINTER_GET_POINTER_VALUE_DEFINE(const Array&,  const Value* &)

#define JSON_POINTER_GET_CONST_C_STR_DEFINE(TARGET_TYPE) \
    JSON_POINTER_GET_TYPE_DEFINE_III(const TARGET_TYPE &, \
                                     const char*&, str_type, \
                                     get_str().c_str, \
                                     JSON_POINTER_TAKE_EMPTY_T, \
                                     val = nullptr;)

// get(const CompoundType&, const char* &)
JSON_POINTER_GET_CONST_C_STR_DEFINE(Value)
JSON_POINTER_GET_CONST_C_STR_DEFINE(Object)
JSON_POINTER_GET_CONST_C_STR_DEFINE(Array)

} // namespace json_spirit
// vim: ts=4 sw=4 sts=4 et:
