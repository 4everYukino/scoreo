project(bjson)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

shared_lib(bjson
    bjson_value.cpp
    dump.cpp
    duplicate.cpp
    filter.cpp
    json_parser.cpp
    json_pointer.cpp
    json_printer.cpp
    json_reader.cpp
    json_spirit_helper.cpp
    json_string_template.cpp
    json_writer.cpp
    load.cpp
    number_to_value.cpp
    update.cpp
    yajl_gen_value.cpp
)

target_compile_definitions(bjson PRIVATE -DJSON_SPIRIT_BUILD_DLL)

# vim: set ts=4 sw=4 sts=4 et:
