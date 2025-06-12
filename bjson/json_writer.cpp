#include "json_writer.h"
#include "yajl_gen_value.h"
#include "scrt/yajl_printer.h"

JSON_Writer::JSON_Writer () :
    gen_ (0)
{
}

JSON_Writer::~JSON_Writer ()
{
    close ();
}

yajl_gen JSON_Writer::open (YAJL_Printer& handler, bool beautify)
{
    close ();
    if ((gen_ = yajl_gen_alloc (0))) {
        yajl_gen_config (gen_, yajl_gen_print_callback, YAJL_Printer::print_s, &handler);
        yajl_gen_config (gen_, yajl_gen_beautify, (int) beautify);
    }

    return gen_;
}

void JSON_Writer::close ()
{
    if (gen_) {
        yajl_gen_free(gen_);
        gen_ = 0;
    }
}

bool JSON_Writer::write (const json_spirit::Value& val, YAJL_Printer& handler,
                         bool beautify)
{
    if (!open (handler, beautify))
        return false;

    bool rc = yajl_gen_value (gen_, val) == yajl_gen_status_ok;
    close ();
    return rc;
}

bool JSON_Writer::write (JSON_Generator& generator, YAJL_Printer& handler,
                         bool beautify)
{
    if (!open (handler, beautify))
        return false;

    bool rc = generator.generate (gen_);
    close ();
    return rc;
}
// vim: set et ts=4 sts=4 sw=4:
