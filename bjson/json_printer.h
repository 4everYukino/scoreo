/// \file json_printer.h
/// \brief JSON spirit printer.
/// \author Bin Lan <lanbin.scutech@gmail.com>
#ifndef JSON_PRINTER_H
#define JSON_PRINTER_H

#include "json_reader.h"
#include "scrt/yajl_printer.h"

class JSON_SPIRIT_Export JSON_Printer: public YAJL_Printer
{
public:
    JSON_Printer(json_spirit::Value* result);
    virtual ~JSON_Printer();

protected:
    virtual void print(const char* str, size_t len);

private:
    JSON_Reader reader_;
};

#endif // !JSON_PRINTER_H
// vim: set ts=4 sw=4 sts=4 et:
