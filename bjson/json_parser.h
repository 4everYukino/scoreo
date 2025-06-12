/*!
 * \file json_parser.h
 * \brief JSON parsing handler.
 * \author Like Ma <likemartinma@gmail.com>
 */
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "json_spirit_export.h"
#include "json_spirit_value.h"

#include "scrt/yajl_handler.h"
#include "scrt/ctor_dtor_macros.h"

#include <stack>
#include <string>

class JSON_SPIRIT_Export JSON_Parser: public YAJL_Handler
{
public:
    DEFAULT_CTOR_DTOR_DECLARES(JSON_Parser);
    void result(json_spirit::Value*);

    virtual bool handle_null_i();
    virtual bool handle_boolean_i(bool val);
    virtual bool handle_number_i(const char* val, size_t len);
    virtual bool handle_string_i(const char* val, size_t len);
    virtual bool handle_start_map_i();
    virtual bool handle_map_key_i(const char* key, size_t len);
    virtual bool handle_end_map_i();
    virtual bool handle_start_array_i();
    virtual bool handle_end_array_i();

    size_t levels() const;
    const std::string& key() const;

protected:
    bool pop_current();

    std::stack<json_spirit::Value*> current_;
    std::string key_;
};

#endif /* JSON_PARSER_H */
