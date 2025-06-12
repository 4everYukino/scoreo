/// \brief Utilities for loading JSON format string
/// \author Jianlong Chen <jianlong99@gmail.com>
#ifndef JSON_SPIRIT_LOAD_H
#define JSON_SPIRIT_LOAD_H

#include "json_reader.h"
#include "scrt/compat_features.h"

class JSON_SPIRIT_Export JSON_Load
{
public:
    JSON_Load(const std::string& str);

    DEFAULT_DTOR_DECLARE(JSON_Load);

#if __cpp_ref_qualifiers >= 200710
    const json_spirit::Value& value() const&;

    json_spirit::Value value() &&;
#else // __cpp_ref_qualifiers < 200710
    const json_spirit::Value& value() const;

    json_spirit::Value value();
#endif // __cpp_ref_qualifiers < 200710

    explicit operator bool () const;

private:
    json_spirit::Value val_;
    bool err_ = false;
};

JSON_SPIRIT_Export bool loads_json(const char* json_str,
                                   size_t len,
                                   json_spirit::Value& json,
                                   int flags = JSON_Reader::FG_LOGGING);

JSON_SPIRIT_Export bool loads_json(const char* json_str,
                                   json_spirit::Value& json);

/// \brief load JSON from file
JSON_SPIRIT_Export bool load_json(const char* path,
                                  json_spirit::Value& json,
                                  int flags = JSON_Reader::FG_LOGGING);

#if defined (__ACE_INLINE__)
#   include "load.inl"
#endif // __ACE_INLINE__
#endif // !JSON_SPIRIT_LOAD_H
// vim: set et ts=4 sts=4 sw=4:
