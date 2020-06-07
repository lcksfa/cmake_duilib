#pragma once;
#include <string>
using namespace std;

inline bool Json_HasField(const Json::Value &root, const string field)
{
    return !root[field].isNull();
}

inline bool Json_GetBool(const Json::Value &root, const string field)
{
    if (root[field].isNull())
        return false;
    else
        return root[field].asBool();
}

inline int Json_GetInt(const Json::Value &root, const string field)
{
    if (root[field].isNull())
        return 0;
    else
        return root[field].asInt();
}

inline string Json_GetString(const Json::Value &root, const string field)
{
    if (root[field].isNull())
        return "";
    else
        return root[field].asString();
}

inline double Json_GetDouble(const Json::Value &root, const string field)
{
    if (root[field].isNull())
        return 0.00;
    return root[field].asDouble();
}
