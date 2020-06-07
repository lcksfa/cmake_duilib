

#ifndef __INCLUDE_FILE_PATH_H__
#define __INCLUDE_FILE_PATH_H__
#pragma once
#include <string>

namespace ckbase
{
bool IsFilePathExist(const std::wstring &path, bool isDirectory);
unsigned _int64 GetFileSize(const std::wstring &filePath);

bool DeleteFile(const std::wstring &filepath);

};     // namespace ckbase
#endif // __INCLUDE_FILE_PATH_H__