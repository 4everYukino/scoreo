/**
 * @file    extract.h
 * @brief   Helper to extract values from json_spirit Objects
 * @author  Xianda <wxianda@gmail.com>
 */

#ifndef JSON_SPIRIT_EXTRACT_H_
#define JSON_SPIRIT_EXTRACT_H_

#include "json_spirit_helper.h"
#include "json_pointer.h"

#include <ace/OS_NS_string.h>
#include <type_traits>

namespace json_spirit {

namespace {
    template <typename U,
              typename T1, typename T2, typename T3,
              template<typename> class TRAITER>
    struct is_same_type_in_3th
        : public std::integral_constant<
            bool,
            std::is_same<T1, typename TRAITER<U>::type>::value ||
            std::is_same<T2, typename TRAITER<U>::type>::value ||
            std::is_same<T3, typename TRAITER<U>::type>::value>
    {
    };

#ifndef __cpp_lib_remove_cvref
    template <typename T>
    using remove_cvref = typename std::remove_cv<typename std::remove_reference<T>::type>;
#else
    template <typename T>
    using remove_cvref = std::remove_cvref<T>;
#endif // __cpp_lib_remove_cvref

    // Value, Object, Array are treated as JSON compound type
    // because they could be used as the root of a JSON document.
    template <typename T>
    struct is_json_compound_type
        : public is_same_type_in_3th<T, Value, Object, Array, remove_cvref>
    {
    };

    template<typename T, typename... Rest>
    struct to_first_assignable
    {
        using type = void;
    };

    template<typename T, typename First, typename... Rest>
    struct to_first_assignable<T, First, Rest...>
    {
        using type = typename std::conditional<
            std::is_assignable<T, First>::value,
            First,
            typename to_first_assignable<T, Rest...>::type>::type;
    };

    template<typename T>
    struct to_assignable_json_types
    {
        using type = typename to_first_assignable<
            T, Value, Object, Array, String_type, secure_string>::type;

        static_assert(!std::is_same<type, void>::value,
                      "T is not assignable from any of the json types");
    };

    template<typename T>
    using to_assignable_json_types_t = typename to_assignable_json_types<T>::type;

    template <typename T>
    struct is_assignable_from_json_types
    {
        static constexpr bool value = !std::is_same<void, typename to_first_assignable<
            T, Value, Object, Array, String_type, secure_string>::type>::value;
    };

    template<typename T>
    struct is_arithmetic_or_enum
    {
        static constexpr bool value = std::is_arithmetic<T>::value
            || std::is_enum<T>::value;
    };
} // anonymous namespace

using Flags = unsigned;

enum : Flags {
    FG_NOTHROW  = 0,
    FG_THROW    = (1 << 0),
};

template <class T>
struct Flaggable_T
{
    T& flags(Flags flags)
    {
        const auto p = static_cast<T*>(this);
        p->flags_ = flags;
        return *p;
    }

    T& throwable(bool val)
    {
        const auto p = static_cast<T*>(this);
        val ? (p->flags_ |= FG_THROW) : (p->flags_ &= ~FG_THROW);
        return *p;
    }
};

template <typename T>
struct JSON_Pointer_Extractor: public Flaggable_T<JSON_Pointer_Extractor<T>>
{
    JSON_Pointer_Extractor(T doc, Flags flags = 0u)
        : doc(std::forward<T>(doc)),
          flags_(flags)
    {
        static_assert(std::is_reference<T>::value,
                      "JSON_Pointer_Extractor: only reference is allowed!");
    }

    JSON_Pointer_Extractor(const JSON_Pointer_Extractor& rhs)
        : doc(std::forward<T>(rhs.doc)),
          flags_(rhs.flags_)
    {
    }

    JSON_Pointer_Extractor(JSON_Pointer_Extractor&& rhs)
        : doc(std::forward<T>(rhs.doc)),
          flags_(rhs.flags_)
    {
    }

    template <typename U>
    JSON_Pointer_Extractor& operator()(const String_type& name,
                                       U& val,
                                       Flags flags)
    {
        static_assert(!std::is_rvalue_reference<T>::value ||
                          !std::is_pointer<U>::value,
                      "JSON_Pointer_Extractor: forbidden to extract a pointer "
                      "from a rvalue JSON!");

        pointer.path(name.c_str(), false);
        if (!pointer.get(std::forward<T>(doc), val) && (flags & FG_THROW))
            throw std::domain_error("JSON Pointer path '" +
                                        name +
                                        "' not exist in JSON!");

        return *this;
    }

    template <typename U>
    JSON_Pointer_Extractor& operator()(const String_type& name, U& val)
    {
        return (*this)(name, val, flags_);
    }

    template <typename U>
    JSON_Pointer_Extractor& operator()(const String_type& name,
                                       OPT_NS::optional<U>& val,
                                       Flags flags)
    {
        static_assert(!std::is_reference<U>::value,
                      "JSON_Pointer_Extractor: forbidden to extract "
                      "a OPT_NS::optional<U>, when U is a reference!");

        pointer.path(name.c_str(), false);
        val = pointer.get<U>(std::forward<T>(doc));
        if (!val && (flags & FG_THROW))
            throw std::domain_error("JSON Pointer path '" +
                                        name +
                                        "' not exist in JSON!");

        return *this;
    }

    template <typename U>
    JSON_Pointer_Extractor& operator()(const String_type& name,
                                       OPT_NS::optional<U>& val)
    {
        return (*this)(name, val, flags_);
    }

    JSON_Pointer pointer;
    T doc;
    Flags flags_ = 0u;
};

template <typename T>
void object_get_may_throw(
        const Object& obj, const String_type& name, T& val, bool throw_)
{
    if (throw_)
        val = std::is_signed<T>::value
              ? static_cast<T>(object_get_int64_or_throw(obj, name))
              : static_cast<T>(object_get_uint64_or_throw(obj, name));
    else
        object_get(obj, name, val);
}

struct Object_Extractor_Base
{
    Object_Extractor_Base(const Object& obj, Flags flags)
        : obj(obj),
          flags_(flags)
    {
    }

#define CONST_OBJECT_EXTRACT_R(type, helper) \
    Object_Extractor_Base& operator()(const String_type& name, \
                                      type& val, \
                                      Flags flags) \
    { \
        if (flags & FG_THROW) \
            val = helper(obj, name); \
        else \
            (void)object_get(obj, name, val); \
\
        return *this; \
    } \
\
    Object_Extractor_Base& operator()(const String_type& name, type& val) \
    { \
        return (*this)(name, val, flags_); \
    }

    CONST_OBJECT_EXTRACT_R(String_type, object_get_str_or_throw)
    CONST_OBJECT_EXTRACT_R(secure_string, object_get_secure_str_or_throw)
    CONST_OBJECT_EXTRACT_R(Array, object_get_array_or_throw)
    CONST_OBJECT_EXTRACT_R(Object, object_get_object_or_throw)
    CONST_OBJECT_EXTRACT_R(Value, object_get_value_or_throw)

    // bool should not be handled by the following template function.
    CONST_OBJECT_EXTRACT_R(bool, object_get_bool_or_throw)

    // For numeric types
    template <typename T,
              typename std::enable_if<is_arithmetic_or_enum<T>::value, int>::type = 0>
    Object_Extractor_Base& operator()(const String_type& name,
                                      T& val,
                                      Flags flags)
    {
        object_get_may_throw(obj, name, val, (flags & FG_THROW));
        return *this;
    }

    // For types assignable from non-arithmetic json types
    template <typename T,
              typename std::enable_if<is_assignable_from_json_types<T>::value, int>::type = 0>
    Object_Extractor_Base& operator()(const String_type& name,
                                      T& val,
                                      Flags flags)
    {
        to_assignable_json_types_t<T> tmp;
        (*this)(name, tmp, flags);
        val = std::move(tmp);
        return *this;
    }

    template <typename T>
    Object_Extractor_Base& operator()(const String_type& name, T& val)
    {
        return (*this)(name, val, flags_);
    }

#define CONST_OBJECT_EXTRACT_P(type, helper) \
    Object_Extractor_Base& operator()(const String_type& name, \
                                      const type*& val, \
                                      Flags flags) \
    { \
        val = flags & FG_THROW ? &helper ## _or_throw(obj, name) \
                               : helper(obj, name); \
        return *this; \
    } \
\
    Object_Extractor_Base& operator()(const String_type& name, \
                                      const type*& val) \
    { \
        return (*this)(name, val, flags_); \
    }

    CONST_OBJECT_EXTRACT_P(String_type, object_get_str)
    CONST_OBJECT_EXTRACT_P(secure_string, object_get_secure_str)
    CONST_OBJECT_EXTRACT_P(Array, object_get_array)
    CONST_OBJECT_EXTRACT_P(Object, object_get_object)
    CONST_OBJECT_EXTRACT_P(Value, object_get_value)

    // Get c_str() of String_type
    Object_Extractor_Base& operator()(const String_type& name,
                                      const char*& val,
                                      Flags flags)
    {
        const auto s = flags & FG_THROW ? &object_get_str_or_throw(obj, name)
                                        : object_get_str(obj, name);
        val = s ? s->c_str() : nullptr;
        return *this;
    }

    Object_Extractor_Base& operator()(const String_type& name,
                                      const char*& val)
    {
        return (*this)(name, val, flags_);
    }

    // Copy to char array from String_type
    Object_Extractor_Base& operator()(const String_type& name,
                                      char* val,
                                      size_t maxlen,
                                      Flags flags)
    {
        const auto s = flags & FG_THROW ? &object_get_str_or_throw(obj, name)
                                        : object_get_str(obj, name);
        ACE_OS::strsncpy(val, s ? s->c_str() : "", maxlen);
        return *this;
    }

    Object_Extractor_Base& operator()(const String_type& name,
                                      char* val,
                                      size_t maxlen)
    {
        return (*this)(name, val, maxlen, flags_);
    }

    const Object& obj;
    Flags flags_ = 0u;
};

/**
 * Extract values or const pointers from a const Object.
 *
 * Example:
 * int age = -1;        // value WILL NOT be initialized if key or type mismatch
 * string name;
 * const Array *skills; // pointer WILL be initialized if key or type mismatch
 * extract(obj)("age", age)         // get value
 *             ("name", name)       // get value
 *             ("skills", skills);  // get const pointer
 */
struct Const_Object_Extractor
    : public Object_Extractor_Base, Flaggable_T<Const_Object_Extractor>
{
    Const_Object_Extractor(const Object& obj, Flags flags = 0u)
        : Object_Extractor_Base(obj, flags)
    {
    }
};

/**
 * Similar to Const_Object_Extractor but also accept non-const pointers.
 *
 * Example:
 *
 * const string *name;  // pointer WILL be initialized if key or type mismatch
 * Array *skills;
 * extract(obj)("name", name)       // get const pointer
 *             ("skills", skills);  // get non-const pointer
 */
struct Object_Extractor
    : public Object_Extractor_Base, Flaggable_T<Object_Extractor>
{
    Object_Extractor(Object& obj, Flags flags = 0u)
        : Object_Extractor_Base(obj, flags)
    {
    }

    using Object_Extractor_Base::operator();

#define OBJECT_EXTRACT_P(type, helper) \
    Object_Extractor& operator()(const String_type& name, \
                                 type* &val, \
                                 Flags flags) \
    { \
        val = flags & FG_THROW \
                  ? &helper ## _or_throw(const_cast<Object&>(obj), name) \
                  : helper(const_cast<Object&>(obj), name); \
        return *this; \
    } \
\
    Object_Extractor& operator()(const String_type& name, type* &val) \
    { \
        return (*this)(name, val, flags_); \
    }

    OBJECT_EXTRACT_P(String_type, object_get_str)
    OBJECT_EXTRACT_P(secure_string, object_get_secure_str)
    OBJECT_EXTRACT_P(Array, object_get_array)
    OBJECT_EXTRACT_P(Object, object_get_object)
    OBJECT_EXTRACT_P(Value, object_get_value)
};

/**
 * Move values from an Object by its rvalue reference.
 *
 * Example:
 *
 * int age = -1;    // value WILL NOT be initialized if key or type mismatch
 * string name;
 * Array skills;
 * extract(move(obj))("age", age)
 *                   ("name", name)
 *                   ("skills", skills);
 */
struct Object_Move_Extractor: public Flaggable_T<Object_Move_Extractor>
{
    Object_Move_Extractor(Object&& obj, Flags flags = 0u)
        : obj(obj),
          flags_(flags)
    {
    }

    // For string, Object, Array, Value
#define OBJECT_MOVE_EXTRACT(type, helper) \
    Object_Move_Extractor& operator()(const String_type& name, \
                                      type& val, \
                                      Flags flags) \
    { \
        if (flags & FG_THROW) { \
            auto& tmp = helper ## _or_throw(obj, name); \
            val = std::move(const_cast<type&>(tmp)); \
        } else { \
            if (auto p = helper(obj, name)) \
                val = std::move(*const_cast<type*>(p)); \
        } \
\
        return *this; \
    } \
\
    Object_Move_Extractor& operator()(const String_type& name, type& val) \
    { \
        return (*this)(name, val, flags_); \
    }

    OBJECT_MOVE_EXTRACT(String_type, object_get_str)
    OBJECT_MOVE_EXTRACT(secure_string, object_get_secure_str)
    OBJECT_MOVE_EXTRACT(Array, object_get_array)
    OBJECT_MOVE_EXTRACT(Object, object_get_object)
    OBJECT_MOVE_EXTRACT(Value, object_get_value)

    // bool should not be handled by the following template function.
    Object_Move_Extractor& operator()(const String_type& name,
                                      bool& val,
                                      Flags flags)
    {
        if (flags & FG_THROW)
            val = object_get_bool_or_throw(obj, name);
        else
            (void)object_get(obj, name, val);

        return *this;
    }

    Object_Move_Extractor& operator()(const String_type& name, bool& val)
    {
        return (*this)(name, val, flags_);
    }

    // For numeric types
    template <typename T,
              typename std::enable_if<is_arithmetic_or_enum<T>::value, int>::type = 0>
    Object_Move_Extractor& operator()(const String_type& name,
                                      T& val,
                                      Flags flags)
    {
        object_get_may_throw(obj, name, val, (flags & FG_THROW));
        return *this;
    }

    // For types assignable from non-arithmetic json types
    template <typename T,
              typename std::enable_if<is_assignable_from_json_types<T>::value, int>::type = 0>
    Object_Move_Extractor& operator()(const String_type& name,
                                      T& val,
                                      Flags flags)
    {
        to_assignable_json_types_t<T> tmp;
        (*this)(name, tmp, flags);
        val = std::move(tmp);
        return *this;
    }

    template <typename T>
    Object_Move_Extractor& operator()(const String_type& name, T& val)
    {
        return (*this)(name, val, flags_);
    }

    Object& obj;
    Flags flags_ = 0u;
};

inline Const_Object_Extractor extract(const Object& obj)
{
    return Const_Object_Extractor(obj);
}

inline Object_Extractor extract(Object& obj)
{
    return Object_Extractor(obj);
}

inline Object_Move_Extractor extract(Object&& obj)
{
    return Object_Move_Extractor(std::move(obj));
}

inline Const_Object_Extractor extract_or_throw(const Object& obj)
{
    return Const_Object_Extractor(obj, FG_THROW);
}

inline Object_Extractor extract_or_throw(Object& obj)
{
    return Object_Extractor(obj, FG_THROW);
}

inline Object_Move_Extractor extract_or_throw(Object&& obj)
{
    return Object_Move_Extractor(std::move(obj), FG_THROW);
}

template<typename T>
inline JSON_Pointer_Extractor<typename std::add_rvalue_reference<T>::type>
pextract(T&& doc)
{
    static_assert(is_json_compound_type<T>::value,
                  "JSON_Pointer_Extractor can only be used for "
                  "Value, Object, Array!");

    return JSON_Pointer_Extractor<typename std::add_rvalue_reference<T>::type>(
               std::forward<T>(doc));
}

template<typename T>
inline JSON_Pointer_Extractor<typename std::add_rvalue_reference<T>::type>
pextract_or_throw(T&& doc)
{
    static_assert(is_json_compound_type<T>::value,
                  "JSON_Pointer_Extractor can only be used for "
                  "Value, Object, Array!");

    return JSON_Pointer_Extractor<typename std::add_rvalue_reference<T>::type>(
               std::forward<T>(doc),
               FG_THROW);
}

template<typename T,
         typename std::enable_if<!is_arithmetic_or_enum<T>::value, int>::type = 0>
T& object_get_any(const Object& obj, const String_type& name);

template<typename T>
T& object_get_any(Object& obj, const String_type& name)
{
    return object_get_any<const T>((const Object&) obj, name);
}

template<>
inline const String_type& object_get_any<const String_type>(
        const Object& obj, const String_type& name)
{
    return object_get_str_or_throw(obj, name);
}

template<>
inline const secure_string& object_get_any<const secure_string>(
        const Object& obj, const String_type& name)
{
    return object_get_secure_str_or_throw(obj, name);
}

template<>
inline const Array& object_get_any<const Array>(
        const Object& obj, const String_type& name)
{
    return object_get_array_or_throw(obj, name);
}

template<>
inline const Object& object_get_any<const Object>(
        const Object& obj, const String_type& name)
{
    return object_get_object_or_throw(obj, name);
}

template<>
inline const Value& object_get_any<const Value>(
        const Object& obj, const String_type& name)
{
    return object_get_value_or_throw(obj, name);
}

template<>
inline Array& object_get_any<Array>(Object& obj, const String_type& name)
{
    return object_get_array_or_throw(obj, name);
}

template<>
inline Object& object_get_any<Object>(Object& obj, const String_type& name)
{
    return object_get_object_or_throw(obj, name);
}

// These two enable extract_object_ptr to be used in if clause
template<typename T>
T* object_get_any_ptr(const Object& obj, const String_type& name)
{
    T* p;
    extract(obj)(name, p);
    return p;
}

template<typename T>
T* object_get_any_ptr(Object& obj, const String_type& name)
{
    T* p;
    extract(obj)(name, p);
    return p;
}

template<typename T>
T object_get_or_default(
        const Object& obj, const String_type& name, const T& default_)
{
    T val;
    return object_get(obj, name, val) ? val : default_;
}

template <typename T> void _do_init(T& t) { t = 0; }
inline void _do_init(String_type&) {}
inline void _do_init(secure_string&) {}
inline void _do_init(Object&) {}
inline void _do_init(Array&) {}

}   // namespace json_spirit

#define EXTRACT_OBJECT(obj, type, name) \
    type name; \
    json_spirit::_do_init(name); \
    json_spirit::object_get(obj, #name, name)

#define EXTRACT_OBJECT_DEFAULT(obj, type, name, default_) \
    type name = json_spirit::object_get_or_default(obj, #name, default_)

#define EXTRACT_OBJECT_REF(obj, type, name) \
    type& name = json_spirit::object_get_any<type>(obj, #name)

#define EXTRACT_OBJECT_PTR(obj, type, name) \
    type* name = json_spirit::object_get_any_ptr<type>(obj, #name)

#endif  // JSON_SPIRIT_EXTRACT_H_
// vim: set et ts=4 sts=4 sw=4:
