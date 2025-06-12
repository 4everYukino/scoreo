#include "json_reader.h"
#if !defined (__ACE_INLINE__)
#   include "json_reader.inl"
#endif /* __ACE_INLINE__ */

#include "scrt/yajl_error.h"
#include "scrt/yajl_default_callbacks.h"
#include <ace/Log_Msg.h>
#include <yajl/yajl_parse.h>

JSON_Reader::JSON_Reader() = default;

JSON_Reader::~JSON_Reader() = default;

bool JSON_Reader::open()
{
    close();
    handle_.reset(yajl_alloc((yajl_callbacks*)get_yajl_default_callbacks(), nullptr, this));
    return bool(handle_);
}

void JSON_Reader::close()
{
    handle_.reset();
    cb_ = CB_NONE;
}

bool JSON_Reader::read(const char* buf, size_t len, int flags)
{
    if (!handle_)
        return false;

    if (buf) {
        if (yajl_parse(handle_.get(),
                       (const unsigned char*)buf,
                       len) == yajl_status_ok)
            return true;
    } else if (yajl_complete_parse(handle_.get()) == yajl_status_ok) {
        return true;
    }

    if (flags & FG_LOGGING)
        ACE_ERROR((LM_ERROR,
                   ACE_TEXT("Failed to parse JSON: %s, callback=%u\n"),
                   YAJL_Error(handle_.get(), true,
                             (const u_char*)buf, len).c_str(),
                   cb_));

    return false;
}

// vim: set ts=4 sw=4 sts=4 et:
