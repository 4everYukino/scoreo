/*!
 * \file json_reader.h
 * \brief It parses JSON chunk by chunk.
 * \author Like Ma <likemartinma@gmail.com>
 */

#ifndef JSON_READER_H
#define JSON_READER_H

#include "json_parser.h"

#include "scrt/auto_yajl.h"

class JSON_SPIRIT_Export JSON_Reader: public JSON_Parser
{
public:
    enum Flag {
        FG_LOGGING = (1 << 0)
    };

    JSON_Reader();
    virtual ~JSON_Reader();

    virtual bool open();
    virtual bool is_open() const;
    virtual void close();
    virtual bool read(const char* buf, size_t len, int flags = FG_LOGGING);
protected:
    auto_yajl_handle handle_;
};

#if defined (__ACE_INLINE__)
#   include "json_reader.inl"
#endif /* __ACE_INLINE__ */

#endif /* JSON_READER_H */
// vim: set ts=4 sw=4 sts=4 et:
