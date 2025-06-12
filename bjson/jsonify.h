/// \file jsonify.h
/// \brief generate the load/dump functions for a plain data class
#ifndef JSON_SPIRIT_JSONIFY_H_
#define JSON_SPIRIT_JSONIFY_H_

#include "json_spirit_value.h"
#include "extract.h"

#include <boost/vmd/is_tuple.hpp>
#include <boost/preprocessor.hpp>
#include <ace/Log_Msg.h>
#include <exception>

/// \brief Get the first element of a tuple.
#define _PP_TUPLE_FIRST(elem) BOOST_PP_TUPLE_ELEM(0, elem)

/// \brief Convert a tuple to comma separated list e.g. (a, b, ,c) => a, b, c.
#define _PP_TUPLE_ENUM(elem) BOOST_PP_SEQ_ENUM(BOOST_PP_TUPLE_TO_SEQ(elem))

#define _PP_ADD_SUFFIX(flag, name) \
    BOOST_PP_IF(flag, BOOST_PP_EXPAND(BOOST_PP_CAT(name, _)), name)

/// \brief Add namespace prefix if the `ns` argument is not empty.
#define _PP_FUNC_ADD_NAMESPACE(ns, name) BOOST_PP_IF(BOOST_PP_IS_EMPTY(ns), name, ns::name)

/// \brief Call the macro named by `mac` argument if the `se` sequence is tuple
/// else call BOOST_PP_EXPAND with `se` as argument.
#define _PP_IF_IS_TUPLE(mac, se) \
    BOOST_PP_IF(BOOST_VMD_IS_TUPLE(se), mac, BOOST_PP_EXPAND)(se)

#define _PP_VARIADIC_2TH(...) BOOST_PP_VARIADIC_ELEM(1, __VA_ARGS__)

#define _PP_VARIADIC_1TH(...) BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)

/// \brief Extract the first element of a tuple or return the element itself
#define _PP_TUPLE_1TH(elem) _PP_IF_IS_TUPLE(_PP_TUPLE_FIRST, elem)

/// \brief Extract the second element of a tuple as flags, or 0
#define _PP_EXTRACT_FLAGS(elem) \
    BOOST_PP_IF(BOOST_VMD_IS_TUPLE(elem), _PP_VARIADIC_2TH elem, 0)

#define _JSON_SPIRIT_TUPLE_SUFFIX(elem) \
    BOOST_PP_CAT(_PP_VARIADIC_1TH elem, _) \
    BOOST_PP_COMMA() \
    BOOST_PP_SEQ_ENUM(BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_REMOVE(elem, 0)))

#define _JSON_SUFFIX_II(name) BOOST_PP_CAT(name, _)

#define _JSON_SPIRIT_WITH_SUFFIX(elem) \
    BOOST_PP_IF(BOOST_VMD_IS_TUPLE(elem), _JSON_SPIRIT_TUPLE_SUFFIX, _JSON_SUFFIX_II)(elem)

#define _JSON_SPIRIT_NO_SUFFIX(elem) \
    _PP_IF_IS_TUPLE(_PP_TUPLE_ENUM, elem)

#define _JSON_SPIRIT_ADD_SUFFIX(flag) \
    BOOST_PP_IF(flag, _JSON_SPIRIT_WITH_SUFFIX, _JSON_SPIRIT_NO_SUFFIX)

/// \brief Generate sequence of this form: "#elem.n1, elem.n1, elem.n2".
#define _JSON_SPIRIT_FIELD_TO_JSON(r, data, elem) \
    BOOST_PP_STRINGIZE(_PP_TUPLE_1TH(elem)) BOOST_PP_COMMA() \
    _JSON_SPIRIT_ADD_SUFFIX(data)(_PP_TUPLE_1TH(elem)) BOOST_PP_COMMA() \
    _PP_EXTRACT_FLAGS(elem)

/// \brief Generate brackets surrended sequence "{ #(elem.n1) , elem }".
#define _JSON_SPIRIT_JSON_TO_FIELD(r, data, i, elem)             \
    { BOOST_PP_STRINGIZE(_PP_TUPLE_1TH(elem)) \
      BOOST_PP_COMMA() BOOST_PP_IF(_PP_VARIADIC_2TH data, BOOST_PP_CAT(_PP_TUPLE_1TH(elem), _), _PP_TUPLE_1TH(elem)) }  \
    BOOST_PP_COMMA_IF(BOOST_PP_LESS(BOOST_PP_INC(i), _PP_TUPLE_1TH(data)))

/// \brief Generate the load function definition.
#define DEFINE_DUMP_JSON_INTRUSIVE(TYPE, NAME, ...)                        \
    TYPE _PP_FUNC_ADD_NAMESPACE(_PP_TUPLE_1TH(NAME), dump)() const         \
    {                                                                      \
        return TYPE {                                                      \
            BOOST_PP_SEQ_FOR_EACH_I(_JSON_SPIRIT_JSON_TO_FIELD,            \
                                    (BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), _PP_EXTRACT_FLAGS(NAME)),   \
                                    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
        };                                                                 \
    }

/// \brief Generate the dump function definition.
#define DEFINE_LOAD_JSON_INTRUSIVE(NAME, ...)                                        \
    bool _PP_FUNC_ADD_NAMESPACE(_PP_TUPLE_1TH(NAME), load)(const json_spirit::Value& json)          \
    {                                                                                \
        try {                                                                        \
            extract_or_throw(json.get_obj())                                         \
                BOOST_PP_SEQ_TRANSFORM(_JSON_SPIRIT_FIELD_TO_JSON,                   \
                                       _PP_EXTRACT_FLAGS(NAME),                \
                                       BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__));       \
        } catch (const std::exception& e) {                                          \
            ACE_ERROR_RETURN((LM_ERROR,                                              \
                              "Failed to load " BOOST_PP_STRINGIZE(_PP_TUPLE_1TH(NAME)) " from JSON: %s\n", e.what()), \
                              false);                                                \
        }                                                                            \
        return true;                                                                 \
    }

/// \brief Generate the load function definition.
#define DEFINE_LOAD_MOVE_JSON_INTRUSIVE(NAME, ...)                                   \
    bool _PP_FUNC_ADD_NAMESPACE(_PP_TUPLE_1TH(NAME), load)(json_spirit::Value json)  \
    {                                                                                \
        try {                                                                        \
            extract_or_throw(std::move(json.get_obj()))                              \
                BOOST_PP_SEQ_TRANSFORM(_JSON_SPIRIT_FIELD_TO_JSON,                   \
                                       _PP_EXTRACT_FLAGS(NAME),                \
                                       BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__));       \
        } catch (const std::exception& e) {                                          \
            ACE_ERROR_RETURN((LM_ERROR,                                              \
                              "Failed to load " BOOST_PP_STRINGIZE(_PP_TUPLE_1TH(NAME)) " from JSON: %s\n", e.what()), \
                              false);                                                \
        }                                                                            \
        return true;                                                                 \
    }

/// \brief How to use the DEFINE_JSONFIY_INTRUSIVE macro.
/// \code{.cpp}
/// // In the person.h header file
/// class Person {
/// public:
///     DECLARE_JSONIFY_INTRUSIVE(json_spirit::Object);
/// private:
///     std::string name;
///     unsigned int age;
///     bool gender;
/// };
///
/// // In the person.cpp implement file:
///
/// DEFINE_JSONIFY_INTRUSIVE(json_spirit::Object, Person, name, age, (gender, JSONIFY_OPTIONAL))
///
/// // The `DEFINE_JSONIFY_INTRUSIVE takes variadic arguments, but the first
/// // and second argument are mandatory and they have special meanings:
/// //   - the first argument is the return type of the `dump` function
/// //   - the second argument is the class or namespace that defined the functions
/// //   - the remain arguments are the fields that the load/dump
/// //     functions will generated.
/// // In the previous example, The macro call DEFINE_JSONIFY_INTRUSIVE(json_spirit::Object, Person, name, age, (gender, JSONIFY_OPTIONAL))
/// // will generate the following function definitions:
/// // bool Person::load(const json_spirit::Value& json)
/// // {
/// //     try {
/// //         extract_or_throw(json.get_obj()) ("name", name) ("age", age) ("gender", gender, false);
/// //     } catch (const std::exception& e) {
/// //         ACE_ERROR_RETURN((LM_ERROR, "Failed to load " "Person" "from JSON: %s\n", e.what()));
/// //     }
/// //     return true;
/// // }
/// //
/// // json_spirit::Object Person::dump() const
/// // {
/// //     return json_spirit::Object {
/// //         { "name", name }, { "age", age }, { "gender", gender }
/// //     };
/// // }
/// //
/// // If you want inline the load/dump function definitions, use
/// // `JSONFIY_NO_CLS_NAME` as the second argument of
/// // DEFINE_JSONIFY_INTRUSIVE, e.g.
/// // DEFINE_JSONIFY_INTRUSIVE(TYPE, JSONIFY_NO_CLS_NAME, ...)
/// // See tests/json_spirit_test/jsonify_test.cpp for examples of usage.
/// // vim: set ts=4 sw=4 sts=4 et:
/// \endcode

#define DEFINE_JSONIFY_INTRUSIVE(TYPE, NAME, ...)       \
    DEFINE_DUMP_JSON_INTRUSIVE(TYPE, NAME, __VA_ARGS__) \
    DEFINE_LOAD_JSON_INTRUSIVE(NAME, __VA_ARGS__)

/// \brief Declare the load/dump functions
#define DECLARE_JSONIFY_INTRUSIVE(TYPE)       \
    bool load(const json_spirit::Value& json); \
    TYPE dump() const

/// \brief Make the generated function definitions without namespace prefix.
#define JSONIFY_NO_CLS_NAME

/// \brief Make the generated memeber variable names have '_' suffix.
#define JSONIFY_UNDERSCORE_SUFFIX 1

/// \brief Make the load function not throw an exception.
#define JSONIFY_OPTIONAL json_spirit::FG_NOTHROW

#endif // JSON_SPIRIT_JSONIFY_H_

// vim: set ts=4 sw=4 sts=4 et:
