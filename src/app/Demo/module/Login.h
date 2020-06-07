#ifndef __INCLUDE_LOGIN_H__
#define __INCLUDE_LOGIN_H__

#pragma once
#include <string>
#include "RestRequest.h"

class Login
{
public:
    bool IsValidPhoneNumber(const std::wstring &phone);
    bool IsValidPassword(const std::wstring &password);
    UnorderedStringMap LoginRequest(const std::string &phone,
                                    const std::string &password);
};
#endif // __INCLUDE_LOGIN_H__