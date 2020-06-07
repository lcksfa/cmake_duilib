#ifndef __INCLUDE_FINDPASSWORD_H__
#define __INCLUDE_FINDPASSWORD_H__

#pragma once
#include "RestRequest.h"
#include <string>

class FindPassword
{
public:
    FindPassword() = default;
    ~FindPassword() = default;

    UnorderedStringMap RequestFetchVerCode(const std::string &phone);
    UnorderedStringMap RequestFindPwd(const std::string &verificationcode,
                                      const std::string &phone,
                                      const std::string &password);
};

#endif // __INCLUDE_FINDPASSWORD_H__