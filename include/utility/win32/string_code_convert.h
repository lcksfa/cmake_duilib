// Multi-byte strings and Unicode strings conversion, etc.
#pragma once

#include "base_config.h"

#include "base_export.h"
#include <string>

namespace ckbase
{
namespace win32
{

// the following functions are used to convert encodings between MBCS and Unicode
// on windows ,unicode is ** utf16,**
BASE_EXPORT std::wstring MBCSToUnicode(const char *input, int code_page = CP_ACP);
BASE_EXPORT std::wstring MBCSToUnicode(const std::string &input, int code_page = CP_ACP);
BASE_EXPORT std::string UnicodeToMBCS(const wchar_t *input, int code_page = CP_ACP);
BASE_EXPORT std::string UnicodeToMBCS(const std::wstring &input, int code_page = CP_ACP);

BASE_EXPORT std::string UTF8ToMBCS(const std::string &input);
BASE_EXPORT std::string MBCSToUTF8(const std::string &input);
BASE_EXPORT std::string UnicodeToUTF8(const std::wstring &input);

} // namespace win32
} // namespace ckbase
