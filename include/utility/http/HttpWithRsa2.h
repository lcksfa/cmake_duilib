#ifndef __INCLUDE_HTTPWITHRSA2_H__
#define __INCLUDE_HTTPWITHRSA2_H__

#pragma once
#include <map>
#include <string>
#include "http/HttpInterface.h"

struct CIKeyHelper {
public:
    virtual const std::string GetAppId() = 0;
    virtual const std::string GetRsa2Key() = 0;
    virtual const std::string GetRequestUrl() = 0;
};

class HttpWithRsa2 : public IHttpInterface
{
public:
    HttpWithRsa2() = default;
    std::string request(const OrderStringMap &inputParms, CIKeyHelper &keyData);

private:
    virtual std::string getRequestMethod() = 0;

private:
    std::string makeRequestParmString(const OrderStringMap &inputParms,
                                      CIKeyHelper &keyData);
    std::string sortReqMapToSignedString(const OrderStringMap &mapReq,
                                         CIKeyHelper &keyData);
};

#endif // __INCLUDE_HTTPWITHRSA2_H__