/// \file json_params_helper.h
/// \author Bin Lan <lanbin.scutech@gmail.com>
#ifndef JSON_PARAMS_HELPERS_H
#define JSON_PARAMS_HELPERS_H

#include "json_spirit_value.h"

template<typename V1>
void params_to_array(json_spirit::Value& result, V1 v1)
{
    result = json_spirit::Array();
    json_spirit::Array& params = result.get_array();
    params.push_back(v1);
}

template<typename V1, typename V2>
void params_to_array(json_spirit::Value& result, V1 v1, V2 v2)
{
    result = json_spirit::Array();
    json_spirit::Array& params = result.get_array();
    params.push_back(v1);
    params.push_back(v2);
}

template<typename V1, typename V2, typename V3>
void params_to_array(json_spirit::Value& result, V1 v1, V2 v2, V3 v3)
{
    result = json_spirit::Array();
    json_spirit::Array& params = result.get_array();
    params.push_back(v1);
    params.push_back(v2);
    params.push_back(v3);
}

template<typename V1, typename V2, typename V3, typename V4>
void params_to_array(json_spirit::Value& result, V1 v1, V2 v2, V3 v3, V4 v4)
{
    result = json_spirit::Array();
    json_spirit::Array& params = result.get_array();
    params.push_back(v1);
    params.push_back(v2);
    params.push_back(v3);
    params.push_back(v4);
}

template<typename V1, typename V2, typename V3, typename V4, typename V5>
void params_to_array(
    json_spirit::Value& result, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5)
{
    result = json_spirit::Array();
    json_spirit::Array& params = result.get_array();
    params.push_back(v1);
    params.push_back(v2);
    params.push_back(v3);
    params.push_back(v4);
    params.push_back(v5);
}

template<typename K1, typename V1>
void params_to_object(json_spirit::Value& result, K1& k1, V1 v1)
{
    result = json_spirit::Object();
    json_spirit::Object& params = result.get_obj();
    params[k1] = v1;
}

template<typename K1, typename V1, typename K2, typename V2>
void params_to_object(json_spirit::Value& result, K1& k1, V1 v1, K2& k2, V2 v2)
{
    result = json_spirit::Object();
    json_spirit::Object& params = result.get_obj();
    params[k1] = v1;
    params[k2] = v2;
}

template<typename K1, typename V1, typename K2, typename V2,
         typename K3, typename V3>
void params_to_object(
    json_spirit::Value& result, K1& k1, V1 v1, K2& k2, V2 v2, K3& k3, V3 v3)
{
    result = json_spirit::Object();
    json_spirit::Object& params = result.get_obj();
    params[k1] = v1;
    params[k2] = v2;
    params[k3] = v3;
}

template<typename K1, typename V1, typename K2, typename V2,
         typename K3, typename V3, typename K4, typename V4>
void params_to_object(
    json_spirit::Value& result, K1& k1, V1 v1, K2& k2, V2 v2, K3& k3, V3 v3,
    K4& k4, V4 v4)
{
    result = json_spirit::Object();
    json_spirit::Object& params = result.get_obj();
    params[k1] = v1;
    params[k2] = v2;
    params[k3] = v3;
    params[k4] = v4;
}

template<typename K1, typename V1, typename K2, typename V2,
         typename K3, typename V3, typename K4, typename V4,
         typename K5, typename V5>
void params_to_object(
    json_spirit::Value& result, K1& k1, V1 v1, K2& k2, V2 v2, K3& k3, V3 v3,
    K4& k4, V4 v4, K5& k5, V5 v5)
{
    result = json_spirit::Object();
    json_spirit::Object& params = result.get_obj();
    params[k1] = v1;
    params[k2] = v2;
    params[k3] = v3;
    params[k4] = v4;
    params[k5] = v5;
}

#endif // !JSON_PARAMS_HELPERS_H
// vim: set ts=4 sw=4 sts=4 et:
