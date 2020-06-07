#ifndef __INCLUDE_SIHE_ACCOUNT_HELPER_H__
#define __INCLUDE_SIHE_ACCOUNT_HELPER_H__

#pragma once
#include <string>
#include "RestRequest.h"

class SiheAccountHelper
{
public:
    SiheAccountHelper() = default;
    ~SiheAccountHelper() = default;

    UnorderedStringMap DoRequestSiheAccountStatus(const std::string &userId);
};
#endif // __INCLUDE_SIHE_ACCOUNT_HELPER_H__