#ifndef __INCLUDE_WP_HTTPREQUEST_H__
#define __INCLUDE_WP_HTTPREQUEST_H__
#pragma once

#include "http/HttpWithDes.h"
class IWPHttpRequest : public HttpWithDes
{
public:
    IWPHttpRequest() = default;
    ~IWPHttpRequest() = default;
    virtual std::string GetTag() = 0;

    UnorderedStringMap request(const UnorderedStringMap &params);

protected:
    UnorderedStringMap ParseResponse(const std::string &response);

    virtual UnorderedStringMap DoParseResponse(const Json::Value &root) = 0;

private:
    virtual std::string GetUrl() override;
};

#endif // __INCLUDE_WP_HTTPREQUEST_H__

class GetInvoicesRequest : public IWPHttpRequest
{
public:
    virtual std::string GetTag() final;

    UnorderedStringMap MakeRequestParams(const std::string &taxNo, std::string &startDate,
                                         std::string &endDate);
    virtual UnorderedStringMap DoParseResponse(const Json::Value &root) final;
};
