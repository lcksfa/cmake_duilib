/*
 * Written by Li Zhao <bw_lizhao AT whty Dot com Dot cn>.
 * On 2020年5月28日
 * This file use for 登录的逻辑代码
 *
 */
#include "Login.h"
#include <regex>

constexpr auto regex_invalid_phone_number =
    L"^(13[0-9]|15[012356789]|17[0135678]|18[0-9]|14[579]|19[89]|166)[0-9]{8}$";
constexpr auto regex_invalid_pwd = L"^[A-Za-z0-9]{6,16}$";

bool Login::IsValidPassword(const std::wstring &password)
{
    std::wregex regexper(regex_invalid_pwd);

    return std::regex_match(password, regexper);
}

UnorderedStringMap Login::LoginRequest(const std::string &phone,
                                       const std::string &password)
{
    CLoginRequest loginRequestInstance;
    auto params = loginRequestInstance.makeRequestParms(phone, password);
    return loginRequestInstance.request(params);
}

bool Login::IsValidPhoneNumber(const std::wstring &phone)
{
    if (phone.length() < 11)
        return false;
    std::wregex regexper(regex_invalid_phone_number);

    return std::regex_match(phone, regexper);
}
