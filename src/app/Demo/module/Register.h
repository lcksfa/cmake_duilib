#ifndef __INCLUDE_REGISTER_H__
#define __INCLUDE_REGISTER_H__

#pragma once
#include <string>
#include <vector>
#include "RestRequest.h"
#include "json/json.h"

class Register
{
private:
    /* data */
public:
    Register(/* args */) = default;
    ~Register() = default;

    bool IsValidTaxNO(const std::wstring &taxno);
    bool IsValidName(const std::wstring &name);
    bool IsValidPhone(const std::wstring &phone);
    bool IsValidVerificationcode(const std::wstring &code);
    bool IsValidPassword(const std::wstring &pwd);

    UnorderedStringMap DoRequest(const RegisterRequestParms &registerParams);
    UnorderedStringMap RequestFetchVerCode(const std::string &phone);

    UnorderedStringMap UploadBusinessLicense(const std::string &path,
                                             std::string &userid);
    void OpenUserServiceProtocol();

    std::vector<std::wstring> GetCityListOf(const std::wstring &provinceName);

    std::wstring GetCityNameByCode(const std::string &citycode);
    std::wstring GetProvinceNameByCode(const std::string &provincecode);

    std::string GetCityCodeByName(const std::string &name);
    std::string GetProvinceCodeByName(const std::string &name);

    std::vector<std::wstring> GetProviceList();

private:
    Json::Value GetDatProviderRoot();
};

#endif // __INCLUDE_REGISTER_H__