// Copyright (c) 2019, BWJF Inc. All rights reserved.

// Multi-byte strings and Unicode strings conversion, etc.
// #include "../pch.h"
#include "win32/string_code_convert.h"
#include "toolset/string_util.h"
#include "toolset\convertUTF.h"
#include "macros.h"

namespace ckbase
{
namespace win32
{

std::wstring MBCSToUnicode(const char *input, int code_page)
{
    std::wstring output;
    int length = ::MultiByteToWideChar(code_page, 0, input, -1, NULL, 0);
    if (length <= 0)
        return output;
    output.resize(length - 1);
    if (output.empty())
        return output;
    ::MultiByteToWideChar(code_page, 0, input, -1, &output[0],
                          static_cast<int>(output.size()));
    return output;
}

std::wstring MBCSToUnicode(const std::string &input, int code_page)
{
    std::wstring output;
    int length = ::MultiByteToWideChar(code_page, 0, input.c_str(),
                                       static_cast<int>(input.size()), NULL, 0);
    output.resize(length);
    if (output.empty())
        return output;
    ::MultiByteToWideChar(code_page, 0, input.c_str(), static_cast<int>(input.size()),
                          &output[0], static_cast<int>(output.size()));
    return output;
}

std::string UnicodeToMBCS(const wchar_t *input, int code_page)
{
    std::string output;
    int length = ::WideCharToMultiByte(code_page, 0, input, -1, NULL, 0, NULL, NULL);
    if (length <= 0)
        return output;
    output.resize(length - 1);
    if (output.empty())
        return output;
    ::WideCharToMultiByte(code_page, 0, input, length - 1, &output[0],
                          static_cast<int>(output.size()), NULL, NULL);
    return output;
}

std::string UnicodeToMBCS(const std::wstring &input, int code_page)
{
    std::string output;
    int length = ::WideCharToMultiByte(
        code_page, 0, input.c_str(), static_cast<int>(input.size()), NULL, 0, NULL, NULL);
    output.resize(length);
    if (output.empty())
        return output;
    ::WideCharToMultiByte(code_page, 0, input.c_str(), static_cast<int>(input.size()),
                          &output[0], static_cast<int>(output.size()), NULL, NULL);
    return output;
}
// warning: can ONLY use in windows system!
std::wstring UTF8ToUTF16(const char *utf8, size_t length)
{
    wchar_t output[4096];
    const UTF8 *src_begin = reinterpret_cast<const UTF8 *>(utf8);
    const UTF8 *src_end = src_begin + length;
    UTF16 *dst_begin = reinterpret_cast<UTF16 *>(output);

    std::wstring utf16;
    while (src_begin < src_end) {
        ConversionResult result =
            ConvertUTF8toUTF16(&src_begin, src_end, &dst_begin,
                               dst_begin + COUNT_OF(output), lenientConversion);

        utf16.append(output, dst_begin - reinterpret_cast<UTF16 *>(output));
        dst_begin = reinterpret_cast<UTF16 *>(output);
        if (result == sourceIllegal || result == sourceExhausted) {
            utf16.clear();
            break;
        }
    }

    return utf16;
}

// warning: can ONLY use in windows system!
std::string UTF16ToUTF8(const wchar_t *utf16, size_t length)
{
    char output[8192];
    const UTF16 *src_begin = reinterpret_cast<const UTF16 *>(utf16);
    const UTF16 *src_end = src_begin + length;
    UTF8 *dst_begin = reinterpret_cast<UTF8 *>(output);

    std::string utf8;
    while (src_begin < src_end) {
        ConversionResult result =
            ConvertUTF16toUTF8(&src_begin, src_end, &dst_begin,
                               dst_begin + COUNT_OF(output), lenientConversion);

        utf8.append(output, dst_begin - reinterpret_cast<UTF8 *>(output));
        dst_begin = reinterpret_cast<UTF8 *>(output);
        if (result == sourceIllegal || result == sourceExhausted) {
            utf8.clear();
            break;
        }
    }

    return utf8;
}

BASE_EXPORT std::string UTF8ToMBCS(const std::string &input)
{
    auto utf16string = UTF8ToUTF16(input.c_str(), input.size());
    return UnicodeToMBCS(utf16string);
}

BASE_EXPORT std::string MBCSToUTF8(const std::string &input)
{
    auto utf16String = MBCSToUnicode(input);
    return UTF16ToUTF8(utf16String.c_str(), utf16String.size());
}

BASE_EXPORT std::string UnicodeToUTF8(const std::wstring &input)
{
    return UTF16ToUTF8(input.c_str(), input.length());
}
} // namespace win32
} // namespace ckbase
