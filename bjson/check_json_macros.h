/// \file check_json_macros.h
/// \brief Check_Json_Macros
/// \author Bin Lan <lanbin.scutech@gmail.com>
#ifndef CHECK_JSON_MACROS_H
#define CHECK_JSON_MACROS_H

#include "json_spirit_value.h"
#include <ace/Log_Msg.h>

#define CHECK_JSON_STR(X) \
if ((X).type() != json_spirit::str_type) { \
    ACE_ERROR((LM_EMERGENCY, \
               "Unexpected error in " __FILE__ ":%d, " \
               #X " is not string!\n", __LINE__)); \
    return; \
}

#define CHECK_JSON_STR_RETURN(X, Y) \
if ((X).type() != json_spirit::str_type) { \
    ACE_ERROR((LM_EMERGENCY, \
               "Unexpected error in " __FILE__ ":%d, " \
               #X " is not string!\n", __LINE__)); \
    return Y; \
}

#define CHECK_JSON_OBJ(X) \
if ((X).type() != json_spirit::obj_type) { \
    ACE_ERROR((LM_EMERGENCY, \
               "Unexpected error in " __FILE__ ":%d, " \
               #X " is not object!\n", __LINE__)); \
    return; \
}

#define CHECK_JSON_OBJ_RETURN(X, Y) \
if ((X).type() != json_spirit::obj_type) { \
    ACE_ERROR((LM_EMERGENCY, \
               "Unexpected error in " __FILE__ ":%d, " \
               #X " is not object!\n", __LINE__)); \
    return Y; \
}

#define CHECK_JSON_ARRAY(X) \
if ((X).type() != json_spirit::array_type) { \
    ACE_ERROR((LM_EMERGENCY, \
               "Unexpected error in " __FILE__ ":%d, " \
               #X " is not array!\n", __LINE__)); \
    return; \
}

#define CHECK_JSON_ARRAY_RETURN(X, Y) \
if ((X).type() != json_spirit::array_type) { \
    ACE_ERROR((LM_EMERGENCY, \
               "Unexpected error in " __FILE__ ":%d, " \
               #X " is not array!\n", __LINE__)); \
    return Y; \
}

#define CHECK_JSON_ARRAY_NOT_EMPTY(X) \
if ((X).empty()) { \
    ACE_ERROR((LM_EMERGENCY, \
               "Unexpected error in " __FILE__ ":%d, " \
               #X " is empty array!\n", __LINE__)); \
    return; \
}

#define CHECK_JSON_ARRAY_NOT_EMPTY_RETURN(X, Y) \
if ((X).empty()) { \
    ACE_ERROR((LM_EMERGENCY, \
               "Unexpected error in " __FILE__ ":%d, " \
               #X " is empty array!\n", __LINE__)); \
    return Y; \
}

#endif // !CHECK_JSON_MACROS_H
// vim: set ts=4 sw=4 sts=4 et:
