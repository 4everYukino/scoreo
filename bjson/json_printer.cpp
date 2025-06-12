#include "json_printer.h"

#include <ace/Log_Msg.h>

using json_spirit::Value;

JSON_Printer::JSON_Printer(Value* result)
{
    if (!reader_.open()) {
        ACE_ERROR((LM_ERROR,
                   "Failed to open JSON_Reader for JSON_Printer.\n"));
        return;
    }

    reader_.result(result);
}

JSON_Printer::~JSON_Printer()
{
    if (!reader_.read(NULL, 0))
        ACE_ERROR((LM_ERROR, "Failed to load json string.\n"));
}

void JSON_Printer::print(const char* str, size_t len)
{
    if (!reader_.is_open())
        return;

    if (!reader_.read(str, len))
        ACE_ERROR((LM_ERROR,
                   "Failed to load json string: '%.*s'\n",
                   len, str));
}

// vim: set ts=4 sw=4 sts=4 et:
