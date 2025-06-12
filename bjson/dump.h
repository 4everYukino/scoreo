#ifndef JSON_SPIRIT_DUMP_H
#define JSON_SPIRIT_DUMP_H

#include "json_spirit_export.h"
#include "json_spirit_value.h"
#include "scrt/ctor_dtor_macros.h"

class JSON_SPIRIT_Export JSON_Dump
{
public:
    using size_type = std::string::size_type;

    JSON_Dump(const json_spirit::Value& json, bool beautify = false);
    DEFAULT_DTOR_DECLARE(JSON_Dump);

    const std::string& str() const;
    const char* c_str() const;
    size_type size() const;
protected:
    std::string val_;
};

/// \brief write json to file
/// \param path path to write the json
/// \param json json to dump
/// \param flags additional flags to create the file
/// \param beautify beautify the output
/// \param perms file permissions of new file
///        default flags are O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC
///        the final file flags will be:
///        O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC | flags
JSON_SPIRIT_Export bool dump_json(const char* path,
                                  const json_spirit::Value& json,
                                  int flags = 0,
                                  bool beautify = false,
                                  int perms = ACE_DEFAULT_FILE_PERMS);

#if defined (__ACE_INLINE__)
#   include "dump.inl"
#endif // __ACE_INLINE__

#endif
