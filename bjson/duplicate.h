/// \file duplicate.h
/// \brief Utilities for duplicate JSON value.
/// \author Yonghong Dong <dongyonghong@scutech.com>
#ifndef JSON_SPIRIT_DUPLICATOR_H
#define JSON_SPIRIT_DUPLICATOR_H

#include "json_spirit_value.h"
#include "scrt/compat_features.h"

class JSON_SPIRIT_Export JSON_Duplicator final
{
public:
    JSON_Duplicator(json_spirit::Object val);

    DEFAULT_DTOR_DECLARE(JSON_Duplicator);

#if __cpp_ref_qualifiers >= 200710
    /// \brief Duplicate a new JSON Object from the specified keys.
    /// \param[in] keys One or more keys that want to duplicate.
    /// \return A new JSON Object.
    json_spirit::Object to_object(const std::initializer_list<const char*>& keys) &;

    /// \brief Duplicate a new JSON Object from the specified keys.
    /// \param[in] keys One or more keys that want to duplicate.
    /// \return A new JSON Object.
    json_spirit::Object to_object(const std::initializer_list<const char*>& keys) &&;

#else // __cpp_ref_qualifiers < 200710
    /// \brief Duplicate a new JSON Object from the specified keys.
    /// \param[in] keys One or more keys that want to duplicate.
    /// \return A new JSON Object.
    json_spirit::Object to_object(const std::initializer_list<const char*>& keys);
#endif // __cpp_ref_qualifiers < 200710

private:
    json_spirit::Object val_;
};

#endif // !JSON_SPIRIT_DUPLICATOR_H
// vim: set et ts=4 sts=4 sw=4:
