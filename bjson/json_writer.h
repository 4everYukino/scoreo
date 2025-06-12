/*!
 * \file json_writer.h
 * \brief JSON serializer.
 * \author Like Ma <likemartinma@gmail.com>
 */

#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include "json_printer.h"
#include "scrt/json_generator.h"

class JSON_SPIRIT_Export JSON_Writer
{
public:
    JSON_Writer();
    ~JSON_Writer();

    struct yajl_gen_t* open(YAJL_Printer& handler, bool beautify = false);
    void close();
    bool write(const json_spirit::Value& obj, YAJL_Printer& handler,
               bool beautify = false);
    bool write(JSON_Generator& generator, YAJL_Printer& handler,
               bool beautify = false);

protected:
    struct yajl_gen_t* gen_;
};

#endif /* JSON_WRITER_H */
// vim: set et ts=4 sts=4 sw=4:
