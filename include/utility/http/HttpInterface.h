#ifndef __INCLUDE_HTTPINTERFACE_H__
#define __INCLUDE_HTTPINTERFACE_H__

#pragma once

#include <map>
#include <string>
#include <functional>
#include <unordered_map>
#include "json/json.h"
using UnorderedStringMap = std::unordered_map<std::string, std::string>;
using OrderStringMap = std::map<std::string, std::string>;

using HttpRequestFuncType = std::function<UnorderedStringMap(void)>;
using HttpResponseFuncType = std::function<void(const UnorderedStringMap &)>;

class ParseResponseHelper
{
public:
    static Json::Value doCommonParse(const std::string &response);
};

class IHttpInterface
{

public:
    virtual ~IHttpInterface() = default;
    // disable copy constructor
    IHttpInterface(IHttpInterface const &) = delete;
    IHttpInterface &operator=(IHttpInterface const &) = delete;

public:
    std::string request(const std::string &requestUrl, const std::string &requestParms,
                        bool IsUseSSL = true);

protected:
    IHttpInterface() = default;
};
#endif // __INCLUDE_HTTPINTERFACE_H__