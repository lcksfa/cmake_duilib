#ifndef __INCLUDE_BASE64_H__
#define __INCLUDE_BASE64_H__

// Base64.h: interface for the CBase64 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <stdio.h>
#include <string>
class CBase64
{
public:
    CBase64();
    virtual ~CBase64();
    static inline bool is_base64(unsigned char c)
    {
        return (isalnum(c) || (c == '+') || (c == '/'));
    };
    std::string base64_encode(unsigned char const *, unsigned int len);
    std::string base64_encode(std::string const &s);
    std::string base64_decode(unsigned char const *, unsigned int len);
    std::string base64_decode(std::string const &s);
};

#endif // __INCLUDE_BASE64_H__