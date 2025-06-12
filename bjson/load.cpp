#include "load.h"
#if !defined (__ACE_INLINE__)
#   include "load.inl"
#endif // __ACE_INLINE__

#include "scrt/check_macros.h"
#include "scrt/compat_open.h"

#include <ace/Log_Msg.h>
#include <ace/Mem_Map.h>

using namespace json_spirit;
using namespace std;

JSON_Load::JSON_Load(const string& str)
    : err_(loads_json(str.c_str(), str.size(), val_, 0))
{
}

DEFAULT_DTOR_DEFINE(JSON_Load);

bool loads_json(const char* json_str, size_t len, Value& json, int flags)
{
    if (!json_str || !*json_str)
        return false;

    JSON_Reader reader;
    if (!reader.open())
        ACE_ERROR_RETURN((LM_ERROR, "Failed to open JSON_Reader\n"), false);

    json = Value::null;
    reader.result(&json);
    if (!reader.read(json_str, len, flags) ||
            !reader.read(nullptr, 0, flags)) {
        if (flags & JSON_Reader::FG_LOGGING)
            ACE_ERROR((LM_ERROR,
                       "Failed to load json string: %.*s\n",
                       len, json_str));

        return false;
    }

    return true;
}

bool loads_json(const char* json_str, Value& json)
{
    return loads_json(json_str, strlen(json_str), json);
}

bool load_json(const char* path, Value& json, int flags)
{
    CHECK_C_STR_RETURN(path, false);

    ACE_Mem_Map map;
    if (map.map(path,
                size_t(-1),
                O_RDONLY,
                ACE_DEFAULT_FILE_PERMS,
                PROT_READ) < 0) {
        if (flags & JSON_Reader::FG_LOGGING)
            ACE_ERROR((LM_ERROR,
                       "Failed to load %s into memory, errno=%d, %m\n",
                       path, ACE_OS::last_error()));

        return false;
    }

#ifndef _WIN32
    enable_fd_cloexec(map.handle());
#endif // !_WIN32

    return loads_json((const char*)map.addr(), map.size(), json);
}

// vim: set et ts=4 sts=4 sw=4:
