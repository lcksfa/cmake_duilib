#ifndef __INCLUDE_RSA2_H__
#define __INCLUDE_RSA2_H__

#pragma once
#include <string>
class CRSA2
{
public:
    static std::string sign(const std::string &content, const std::string &pKey);
};
#endif // __INCLUDE_RSA2_H__