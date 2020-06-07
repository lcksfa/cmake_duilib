#ifndef __INCLUDE_HTTPWITHDES_H__
#define __INCLUDE_HTTPWITHDES_H__
#pragma once
#include <string>
#include "http/HttpInterface.h"
#include "json/json.h"

class HttpWithDes : public IHttpInterface
{
public:
    HttpWithDes() = default;
    std::string request(const UnorderedStringMap &requestParms);

protected:
    virtual std::string GetUrl() = 0;
    std::string makeRequestParmString(const UnorderedStringMap &inputParms);

private:
    //解密
    std::string Decrypt(const std::string &src);
    //加密
    std::string Encrypt(const std::string &src);
};

#endif // __INCLUDE_HTTPWITHDES_H__