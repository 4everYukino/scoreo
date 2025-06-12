/**
 * @file    assign.h
 * @brief   Helper to create or insert into json_spirit Objects & Arrays
 * @author  Xianda <wxianda@gmail.com>
 */

#ifndef JSON_SPIRIT_ASSIGN_H_
#define JSON_SPIRIT_ASSIGN_H_

#include "json_spirit_value.h"
#include "json_pointer.h"
#include "scrt/compat_features.h"

#include <boost/range/iterator_range.hpp>

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

namespace json_spirit {

/// Wrapper denoting that invalid value should not be inserted.
template <typename T>
struct Skip_Inserting_Tag
{
    Skip_Inserting_Tag(T&& value, bool valid)
        : value(std::forward<T>(value))
        , valid(valid)
    {
    }

    T&& value;
    const bool valid;
};

struct Skip_Inserting_Helper
{
    bool valid;
};

/**
 * Helper to create \a Skip_Inserting_Tag instance by operator '|'.
 *
 * Example:
 *
 * insert(obj)("optional_key", value_may_be_empty | skip_if(value.empty()));
 */
inline Skip_Inserting_Helper skip_if(bool cond)
{
    return Skip_Inserting_Helper{!cond};
}

template <typename T>
inline Skip_Inserting_Tag<T> operator|(T&& t, Skip_Inserting_Helper helper)
{
    return Skip_Inserting_Tag<T>(std::forward<T>(t), helper.valid);
}

/**
 * Helper to create \a Skip_Inserting_Tag instance by function.
 *
 * Example:
 *
 * insert(obj)("optional_key", skip_if(value_may_be_empty, value.empty()));
 */
template <typename T>
inline Skip_Inserting_Tag<T> skip_if(T&& t, bool cond)
{
    return Skip_Inserting_Tag<T>(std::forward<T>(t), !cond);
}

/// Wrapper denoting that empty string/Object/Array should not be inserted.
template <typename T>
struct Skip_Empty_Tag : public Skip_Inserting_Tag<T>
{
    explicit Skip_Empty_Tag(T&& value)
        : Skip_Inserting_Tag<T>(std::forward<T>(value), !value.empty())
    {
    }
};

/**
 * Helper to create \a Skip_Empty_Tag instance by function or operator '|'.
 *
 * Example:
 *
 * insert(obj)("optional_key", skip_empty(value_may_be_empty));
 * insert(obj)("optional_key", value_may_be_empty | skip_empty);
 */
struct Skip_Empty_Helper
{
    template <typename T>
    Skip_Empty_Tag<T> operator()(T&& t) const
    {
        return Skip_Empty_Tag<T>(std::forward<T>(t));
    }
};

template <typename T>
inline Skip_Empty_Tag<T> operator|(T&& t, Skip_Empty_Helper)
{
    return Skip_Empty_Tag<T>(std::forward<T>(t));
}

const Skip_Empty_Helper skip_empty;

/// Wrapper denoting that null pointer should be ignored.
template <typename T>
struct Skip_NULL_Tag
{
    T* ptr;
};

/**
 * Helper to create \a Skip_NULL_Tag instance by function or operator '|'.
 *
 * Example:
 *
 * insert(obj)("optional_key", skip_null(pointer_may_be_null));
 * insert(obj)("optional_key", pointer_may_be_null | skip_null);
 *
 * NOTE: Should not use with 'char *'.
 */
struct Skip_NULL_Helper
{
    template <typename T>
    Skip_NULL_Tag<T> operator()(T* p) const
    {
        return Skip_NULL_Tag<T>{p};
    }
};

template <typename T>
inline Skip_NULL_Tag<T> operator|(T* p, Skip_NULL_Helper)
{
    return Skip_NULL_Tag<T>{p};
}

const Skip_NULL_Helper skip_null;

struct Pointer_Inserter
{
    Pointer_Inserter(Value& val) : val(val)
    {
    }

    #define POINTER_INSERTER_DEFINE(TYPE) \
    Pointer_Inserter& operator()(const String_type& name, TYPE value) \
    { \
        pointer.path(name.c_str()); \
        pointer.set(val, value); \
        return *this; \
    }

    POINTER_INSERTER_DEFINE(bool)
    POINTER_INSERTER_DEFINE(double)
    POINTER_INSERTER_DEFINE(const Object&)
    POINTER_INSERTER_DEFINE(const Array&)
    POINTER_INSERTER_DEFINE(const Value&)
    POINTER_INSERTER_DEFINE(const std::string&)
    POINTER_INSERTER_DEFINE(const secure_string&)
    POINTER_INSERTER_DEFINE(const char*)
    POINTER_INSERTER_DEFINE(int)
    POINTER_INSERTER_DEFINE(unsigned)
    POINTER_INSERTER_DEFINE(int64_t)
    POINTER_INSERTER_DEFINE(uint64_t)

    #undef POINTER_INSERTER_DEFINE

    #define POINTER_MOVEABLE_INSERTER_DEFINE(TYPE) \
    Pointer_Inserter& operator()(const String_type& name, TYPE value) \
    { \
        pointer.path(name.c_str()); \
        pointer.set(val, std::move(value)); \
        return *this; \
    }

    POINTER_MOVEABLE_INSERTER_DEFINE(Object&&)
    POINTER_MOVEABLE_INSERTER_DEFINE(Array&&)
    POINTER_MOVEABLE_INSERTER_DEFINE(Value&&)
    POINTER_MOVEABLE_INSERTER_DEFINE(std::string&&)
    POINTER_MOVEABLE_INSERTER_DEFINE(secure_string&&)

    #undef POINTER_MOVEABLE_INSERTER_DEFINE

    template<typename T>
    Pointer_Inserter& operator()(const String_type& name, Skip_Inserting_Tag<T> wrapper)
    {
        if (wrapper.valid)
            (*this).operator()(name, std::forward<T>(wrapper.value));

        return *this;
    }

    operator Value&() const
    {
        return val;
    }

    Value& val;
    JSON_Pointer pointer;
};

struct Object_Inserter
{
    Object_Inserter(Object& obj) : obj(obj)
    {
    }

#if __cpp_ref_qualifiers >= 200710
    template<typename T>
    Object_Inserter& operator()(const String_type& name, Skip_Inserting_Tag<T> wrapper) &
    {
        if (wrapper.valid)
            (*this).operator()(name, std::forward<T>(wrapper.value));

        return *this;
    }

    template<typename T>
    Object_Inserter& operator()(const String_type& name, Skip_NULL_Tag<T> wrapper) &
    {
        if (wrapper.ptr)
            this->operator()(name, *wrapper.ptr);

        return *this;
    }

    Object_Inserter& operator()(const String_type& name, Value val) &
    {
        obj[name] = std::move(val);
        return *this;
    }

    template <class T>
    Object_Inserter& operator()(const std::map<String_type, T>& val) &
    {
        obj.insert(val.begin(), val.end());
        return *this;
    }

    template <typename... Ts>
    Object_Inserter&& operator()(Ts&&... args) &&
    {
        return std::move(operator()(std::forward<Ts>(args)...));
    }
#else // __cpp_ref_qualifiers < 200710
    template<typename T>
    Object_Inserter&& operator()(const String_type& name, Skip_Inserting_Tag<T> wrapper)
    {
        if (wrapper.valid)
            (*this).operator()(name, std::forward<T>(wrapper.value));

        return std::move(*this);
    }

    template<typename T>
    Object_Inserter&& operator()(const String_type& name, Skip_NULL_Tag<T> wrapper)
    {
        if (wrapper.ptr)
            this->operator()(name, *wrapper.ptr);

        return std::move(*this);
    }

    Object_Inserter&& operator()(const String_type& name, Value val)
    {
        obj[name] = std::move(val);
        return std::move(*this);
    }

    template <class T>
    Object_Inserter&& operator()(const std::map<String_type, T>& val)
    {
        obj.insert(val.begin(), val.end());
        return std::move(*this);
    }
#endif // __cpp_ref_qualifiers < 200710

    operator const Object&() const
    {
        return obj;
    }

    operator Object&()
    {
        return obj;
    }

    operator const Object*() const
    {
        return &obj;
    }

    operator Object*()
    {
        return &obj;
    }

    Object& obj;
};

struct Object_Maker : public Value, public Object_Inserter
{
    Object_Maker() : Value(Object()), Object_Inserter(get_obj())
    {
    }

#if __cpp_ref_qualifiers >= 200710
    template <typename... Ts>
    Object_Maker& operator()(Ts&&... args) &
    {
        Object_Inserter::operator()(std::forward<Ts>(args)...);
        return *this;
    }

    template <typename... Ts>
    Object_Maker&& operator()(Ts&&... args) &&
    {
        return std::move(operator()(std::forward<Ts>(args)...));
    }
#else // __cpp_ref_qualifiers < 200710
    template <typename... Ts>
    Object_Maker&& operator()(Ts&&... args)
    {
        Object_Inserter::operator()(std::forward<Ts>(args)...);
        return std::move(*this);
    }
#endif // __cpp_ref_qualifiers < 200710
};

// Do not use macro to avoid conflict
inline Object_Inserter insert(Object& obj)
{
    return Object_Inserter(obj);
}

// Do not use macro to avoid conflict
inline Pointer_Inserter pinsert(Value& val)
{
    return Pointer_Inserter(val);
}

template <class T>
Object& operator+=(Object& obj, const std::map<String_type, T>& val)
{
    obj.insert(val.begin(), val.end());
    return obj;
}

inline Array& operator<<(Array& arr, Value val)
{
    arr.push_back(std::move(val));
    return arr;
}

// The second argument is defaulted to size_t, while T::size() of containers
// always return size_t. Otherwise the following specialization will not match.
template <class T, class = size_t>
struct size_method_helper
{
    /**
     * For types not having method \a size(), return 0.
     */
    static size_t size(const T&)
    {
        return 0;
    }
};

template <class T>
struct size_method_helper<T, decltype(std::declval<T>().size())>
{
    /**
     * For types having method \a size(), return value of \a size().
     */
    static size_t size(const T& t)
    {
        return t.size();
    }
};

/**
 * Copy elements of an iterable object to Array.
 */
template <class T>
Array& operator+=(Array& arr, const T& val)
{
    if (const auto size = size_method_helper<T>::size(val))
        arr.reserve(arr.size() + size);
    arr.insert(arr.end(), val.begin(), val.end());
    return arr;
}

/**
 * Move elements from container to Array.
 *
 * The container should provide method \a size(). So this operator does not aim
 * to 'generators' like boost adaptors.
 */

/**
 * The following conditions are needed to avoid accidental moved of a boost::range.
 * e.g.
 *    Array arr1{...};
 *    Array arr2{...};
 *    arr2 += arr1 | filter_type(string);
 *            |
 *            the operand is a rvalue binding of type boost::filtered_range<...>;
*/
template <typename T,
          class = typename std::enable_if<
                               !std::is_lvalue_reference<T>::value &&
                               !std::is_base_of<boost::iterator_range<typename T::iterator>, T>::value
                            >::type,
          class = decltype(std::declval<T>().size())>
Array& operator+=(Array& arr, T&& val)
{
    arr.reserve(arr.size() + val.size());
    std::move(val.begin(), val.end(), std::back_inserter(arr));
    return arr;
}

struct Array_Maker : public Value
{
    Array_Maker() : Value(Array()), arr(get_array())
    {
    }

#if __cpp_ref_qualifiers >= 200710
    Array_Maker& operator()(Value val) &
    {
        arr.push_back(std::move(val));
        return *this;
    }

    template<typename T>
    Array_Maker& operator()(Skip_Inserting_Tag<T> wrapper) &
    {
        if (wrapper.valid)
            (*this).operator()(std::forward<T>(wrapper.value));

        return *this;
    }

    /**
     * Build Array by elements in [first, last), reserve capacity if size is provided.
     */
    template <class Iter>
    Array_Maker& operator()(Iter first, Iter last, size_t size = 0) &
    {
        if (size)
            arr.reserve(arr.size() + size);
        arr.insert(arr.end(), first, last);
        return *this;
    }

    /**
     * Build Array by elements in an iterable object.
     */
    // Enable this only if T cannot be converted to Value, and has method begin().
    // Without std::declval, compiler complains 'decltype cannot resolve address
    // of overloaded function'.
    template <typename T,
              class = typename std::enable_if<!std::is_convertible<T, Value>::value>::type,
              class = decltype(std::declval<T>().begin())>
    Array_Maker& operator()(const T& val) &
    {
        return operator()(val.begin(), val.end(), size_method_helper<T>::size(val));
    }

    /**
     * Move elements from container to Array.
     *
     * Method \a size() is required for type \a T, so 'generators' like boost
     * adaptors, which usually hold lvalue reference of elements of other
     * containers, will not be deduced to this template.
     */
    // Enable this only if T cannot be converted to Value, and has methods
    // begin() & size(), and val is rvalue ref.
    template <typename T,
              class = typename std::enable_if<!std::is_convertible<T, Value>::value>::type,
              class = typename std::enable_if<!std::is_lvalue_reference<T>::value>::type,
              class = decltype(std::declval<T>().begin()),
              class = decltype(std::declval<T>().size())>
    Array_Maker& operator()(T&& val) &
    {
        arr.reserve(arr.size() + val.size());
        std::move(val.begin(), val.end(), std::back_inserter(arr));
        return *this;
    }

    template <typename... Ts>
    Array_Maker&& operator()(Ts&&... args) &&
    {
        return std::move(operator()(std::forward<Ts>(args)...));
    }
#else // __cpp_ref_qualifiers < 200710
    Array_Maker&& operator()(Value val)
    {
        arr.push_back(std::move(val));
        return std::move(*this);
    }

    template<typename T>
    Array_Maker& operator()(Skip_Inserting_Tag<T> wrapper)
    {
        if (wrapper.valid)
            (*this).operator()(std::forward<T>(wrapper.value));

        return *this;
    }

    /**
     * Build Array by elements in [first, last), reserve capacity if size is provided.
     */
    template <class Iter>
    Array_Maker&& operator()(Iter first, Iter last, size_t size = 0)
    {
        if (size)
            arr.reserve(arr.size() + size);
        arr.insert(arr.end(), first, last);
        return std::move(*this);
    }

    /**
     * Build Array by elements in a container with method \a size().
     */
    // Enable this only if T cannot be converted to Value, and has method begin().
    // Without std::declval, compiler complains 'decltype cannot resolve address
    // of overloaded function'.
    template <typename T,
              class = typename std::enable_if<!std::is_convertible<T, Value>::value>::type,
              class = decltype(std::declval<T>().begin())>
    Array_Maker&& operator()(const T& val)
    {
        return operator()(val.begin(), val.end(), size_method_helper<T>::size(val));
    }

    /**
     * Move elements from container to Array.
     *
     * Method \a size() is required for type \a T, so 'generators' like boost
     * adaptors, which usually hold lvalue reference of elements of other
     * containers, will not be deduced to this template.
     */
    // Enable this only if T cannot be converted to Value, and has methods
    // begin() & size(), and val is rvalue ref.
    template <typename T,
              class = typename std::enable_if<!std::is_convertible<T, Value>::value>::type,
              class = typename std::enable_if<!std::is_lvalue_reference<T>::value>::type,
              class = decltype(std::declval<T>().begin()),
              class = decltype(std::declval<T>().size())>
    Array_Maker&& operator()(T&& val)
    {
        arr.reserve(arr.size() + val.size());
        std::move(val.begin(), val.end(), std::back_inserter(arr));
        return std::move(*this);
    }
#endif // __cpp_ref_qualifiers < 200710

    operator const Array&() const
    {
        return arr;
    }

    operator Array&()
    {
        return arr;
    }

    operator const Array*() const
    {
        return &arr;
    }

    operator Array*()
    {
        return &arr;
    }

    Array &arr;
};

inline Array& operator+=(Array& arr, const Array_Maker& mak)
{
    return arr += mak.arr;
}

inline Array& operator+=(Array& arr, Array_Maker&& mak)
{
    return arr += std::move(mak.arr);
}

inline Object& to_new_object(Value& val)
{
    return (val = Object()).get_obj();
}

inline Array& to_new_array(Value& val)
{
    return (val = Array()).get_array();
}

inline String_type& to_new_string(Value& val)
{
    return (val = String_type()).get_str();
}

inline secure_string& to_new_secure_string(Value& val)
{
    return (val = secure_string()).get_secure_str();
}

inline Object& to_object(Value& val)
{
    return val.type() == json_spirit::obj_type ? val.get_obj()
                                               : to_new_object(val);
}

inline Object to_object(Value&& val)
{
    return std::move(to_object(val));
}

inline Array& to_array(Value& val)
{
    return val.type() == json_spirit::array_type ? val.get_array()
                                                 : to_new_array(val);
}

inline Array to_array(Value&& val)
{
    return std::move(to_array(val));
}

inline String_type& to_string(Value& val)
{
    return val.type() == json_spirit::str_type ? val.get_str()
                                               : to_new_string(val);
}

inline String_type to_string(Value&& val)
{
    return std::move(to_string(val));
}

inline secure_string& to_secure_string(Value& val)
{
    return val.type() == json_spirit::secure_str_type
           ? val.get_secure_str()
           : to_new_secure_string(val);
}

inline secure_string to_secure_string(Value&& val)
{
    return std::move(to_secure_string(val));
}

inline Object& append_object(Array& arr, const Object& o)
{
    return arr.insert(arr.end(), o)->get_obj();
}

inline Object& append_new_object(Array& arr)
{
    return arr.insert(arr.end(), Object())->get_obj();
}

inline Array& append_array(Array& arr, const Array& a)
{
    return arr.insert(arr.end(), a)->get_array();
}

inline Array& append_new_array(Array& arr)
{
    return arr.insert(arr.end(), Array())->get_array();
}

}   // namespace json_spirit

/**
 * Return an Object from key-values.
 *
 * Use initializer 'Object{...}' if possible. However, OBJECT_OF is useful in
 * some cases. Example:
 *
 * OBJECT_OF(key, value | skip_if(cond))    // use filters
 *          (instance_of_std_map);          // copied from std::map
 */
#define OBJECT_OF(...) json_spirit::Object_Maker()(__VA_ARGS__)

/**
 * Return an Array from values.
 *
 * Use initializer 'Array{...}' if possible. However, ARRAY_OF is useful in some
 * cases. Example:
 *
 * ARRAY_OF(value | skip_if(cond))  // use filters
 *         (iterable)               // copied from its begin() to end()
 *         (move(iterable));        // moved from its begin() to end()
 */
#define ARRAY_OF(...) json_spirit::Array_Maker()(__VA_ARGS__)

#endif  // JSON_SPIRIT_ASSIGN_H_
// vim: set et ts=4 sts=4 sw=4:
