#include "dump.h"
#if !defined (__ACE_INLINE__)
#   include "dump.inl"
#endif /* __ACE_INLINE__ */

#include "json_writer.h"
#include "scrt/yajl_ostream_printer.h"
#include "scrt/yajl_file_printer.h"
#include <ace/Log_Msg.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <sstream>

using json_spirit::Value;
using namespace std;

DEFAULT_DTOR_DEFINE(JSON_Dump)

JSON_Dump::JSON_Dump(const Value& json, bool beautify)
{
    stringstream ss;
    YAJL_OStream_Printer printer;
    if (printer.open(&ss)) {
        JSON_Writer writer;
        if (!writer.write(json, printer, beautify))
            ACE_ERROR((LM_ERROR, "Failed to dump json object\n"));
    } else {
        ACE_ERROR((LM_ERROR, "Failed to open YAJL_OStream_Printer\n"));
    }

    val_ = ss.str();
}

bool dump_json(const char* path,
               const Value& json,
               int flags,
               bool beautify,
               int perms)
{
    ACE_OS::unlink(path);

    YAJL_File_Printer printer;
    if (!printer.open(path, flags, perms))
        return false;

    if (!JSON_Writer().write(json, printer, beautify))
        ACE_ERROR_RETURN((LM_ERROR,
                          "Failed to write json to '%C'\n",
                          path),
                          false);

    return true;
}

// vim: set ts=4 sw=4 sts=4 et:
