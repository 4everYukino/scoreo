/// \file filter.h
/// \brief Filter json_spirit Array or Object by condition.
/// \author Xianda <wxianda@gmail.com>

#ifndef JSON_SPIRIT_FILTER_H_
#define JSON_SPIRIT_FILTER_H_

#include "json_spirit_value.h"

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <iterator>

namespace json_spirit {

/**
 * Filter Value type with boost::adaptors::filtered.
 *
 * Using with boost::adaptors::filtered directly is tedious, use \a filter_type
 * instead.
 */
struct Type_Checker
{
    bool operator()(const Value &v) const
    {
        return v.type() == type;
    }

    bool operator()(const Object::value_type &v) const
    {
        return v.second.type() == type;
    }

    const Value_type type;
};

using Type_Filter = boost::range_detail::filter_holder<Type_Checker>;

/**
 * Simplify the usage of \a Type_Checker.
 *
 * Example:
 *
 * for (auto &i : arr | filter_type(str_type)) {
 *     // Without the filter, we used to write:
 *     // if (i.type() != str_type) continue;
 *
 *     // Handle str_type items...
 * }
 */
inline Type_Filter filter_type(Value_type type)
{
    return boost::range_detail::filter_holder<Type_Checker>(Type_Checker{type});
}

// HP-UX GCC 4.7 does not fully support decltype. Define 'result' type for the
// following functors.
#ifdef BOOST_RESULT_OF_USE_TR1
#  define FUNCTOR_RESULT_DEFINE(TYPE) \
    template<class Sig> struct result; \
\
    template<typename This> \
    struct result<This(const Value&)> { \
        typedef const TYPE& type; \
    }; \
\
    template<typename This> \
    struct result<This(Value&)> { \
        typedef TYPE& type; \
    }; \
\
    template<typename This> \
    struct result<This(Value&&)> { \
        typedef TYPE&& type; \
    };
#else
#  define FUNCTOR_RESULT_DEFINE(TYPE)
#endif

// Generate 'TYPE& get_type_helper(Value&)' & 'const TYPE& get_type_helper(const Value&)'
#define GET_CLASS_HELPER(TYPE, METHOD) \
const struct Get_##TYPE##_Helper { \
    const TYPE& operator()(const Value &val) const { \
        return val.get_##METHOD(); \
    } \
\
    TYPE& operator()(Value &val) const { \
        return val.get_##METHOD(); \
    } \
 \
    TYPE&& operator()(Value &&val) const { \
        return static_cast<TYPE&&>(val.get_##METHOD()); \
    } \
\
    FUNCTOR_RESULT_DEFINE(TYPE) \
} get_##METHOD##_helper;

GET_CLASS_HELPER(Object, obj)
GET_CLASS_HELPER(Array, array)
GET_CLASS_HELPER(String_type, str)
GET_CLASS_HELPER(secure_string, secure_str)

// Generate 'int get_int_helper(const Value&)', etc.
#define GET_ARITHMETIC_HELPER(METHOD) \
inline decltype(std::declval<const Value>().get_##METHOD()) get_##METHOD##_helper(const Value &val) \
{ \
    return val.get_##METHOD(); \
}

GET_ARITHMETIC_HELPER(bool)
GET_ARITHMETIC_HELPER(int)
GET_ARITHMETIC_HELPER(uint)
GET_ARITHMETIC_HELPER(int64)
GET_ARITHMETIC_HELPER(uint64)
GET_ARITHMETIC_HELPER(real)

template<class Pred, class Conv>
struct Pred_Conv_Pair
{
    Pred pred;
    Conv conv;
};

template<class Pred, class Conv>
inline Pred_Conv_Pair<Pred, Conv> make_pred_conv_pair(Pred pred, Conv conv)
{
    return Pred_Conv_Pair<Pred, Conv>{pred, conv};
}

template<class, class = void>
struct is_range_iterator : std::false_type{};

template<class T>
struct is_range_iterator<T, typename std::enable_if<std::is_base_of<boost::iterator_range<typename T::iterator>, T>::value>::type> : std::true_type{};

static_assert(is_range_iterator<boost::iterator_range<int*>>::value, "is_range_iterator<boost::iterator_range<int*>>");
static_assert(!is_range_iterator<Value>::value, "!is_range_iterator<Value>");

template<class Range,
         class Pred,
         class Conv,
         typename std::enable_if<std::is_lvalue_reference<Range>::value || is_range_iterator<Range>::value, bool>::type = true
        >
inline boost::iterator_range<
        boost::transform_iterator<
            Conv,
            boost::filter_iterator<
                Pred,
                typename boost::range_iterator<Range>::type
                >
            >
        >
operator|(Range&& r, const Pred_Conv_Pair<Pred, Conv>& f)
{
    using boost::make_filter_iterator;
    using boost::make_iterator_range;
    using boost::make_transform_iterator;
    return make_iterator_range(
               make_transform_iterator(
                   make_filter_iterator(f.pred, r.begin(), r.end()), f.conv),
               make_transform_iterator(
                   make_filter_iterator(f.pred, r.end(), r.end()), f.conv)
           );
}

template<class Range,
         class Pred,
         class Conv,
         typename std::enable_if<!std::is_reference<Range>::value
                                 && !is_range_iterator<Range>::value,
                                 bool>::type = true
        >
inline boost::iterator_range<
               boost::transform_iterator<
                   Conv,
                   boost::filter_iterator<
                       Pred,
                       typename std::move_iterator<typename Range::iterator>
                   >
               >
       >
operator|(Range&& r, const Pred_Conv_Pair<Pred, Conv>& f)
{
    using boost::make_filter_iterator;
    using boost::make_iterator_range;
    using boost::make_transform_iterator;
    using std::make_move_iterator;

    return make_iterator_range(
               make_transform_iterator(
                   make_filter_iterator(f.pred,
                                        make_move_iterator(r.begin()),
                                        make_move_iterator(r.end())),
                                        f.conv),
               make_transform_iterator(
                   make_filter_iterator(f.pred,
                                        make_move_iterator(r.end()),
                                        make_move_iterator(r.end())),
                                        f.conv)
           );
}

/**
 * \defgroup Transformers
 *
 * Filter and convert Array items to specified type.
 *
 * Example:
 *
 * for (auto &i : arr | get_obj) {
 *     // Without the transformer, we used to write:
 *     // if (i.type() != obj_type) continue;
 *     // auto &obj = i.get_obj();
 *
 *     // Handle i as a 'Object&' instance.
 * }
 *
 * {@
 */
// decltype(function) returns the function type, but we can only use the function
// pointer instead.
#define FILTER_HELPER_TYPE(TYPE) \
    Pred_Conv_Pair< \
        Type_Checker, \
        typename std::conditional< \
            std::is_function<decltype(get_##TYPE##_helper)>::value, \
            typename std::add_pointer<decltype(get_##TYPE##_helper)>::type, \
            typename std::remove_const<decltype(get_##TYPE##_helper)>::type \
        >::type \
    >

#define FILTER_HELPER_DECL(TYPE) \
JSON_SPIRIT_Export extern const FILTER_HELPER_TYPE(TYPE) get_##TYPE;

FILTER_HELPER_DECL(obj)
FILTER_HELPER_DECL(array)
FILTER_HELPER_DECL(str)
FILTER_HELPER_DECL(secure_str)
FILTER_HELPER_DECL(bool)
FILTER_HELPER_DECL(int)
FILTER_HELPER_DECL(uint)
FILTER_HELPER_DECL(int64)
FILTER_HELPER_DECL(uint64)
FILTER_HELPER_DECL(real)
/// @}

}   // namespace json_spirit

#endif  // JSON_SPIRIT_FILTER_H_
// vim: set ts=4 sw=4 sts=4 et:
