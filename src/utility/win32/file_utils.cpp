#include "win32\file_utils.h"
#include "windows.h"

namespace ckbase
{
bool IsFilePathExist(const std::wstring &path, bool isDirectory)
{
    const DWORD file_attr = ::GetFileAttributesW(path.c_str());
    if (file_attr != INVALID_FILE_ATTRIBUTES) {
        if (isDirectory)
            return (file_attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
        else
            return true;
    }
    return false;
}

unsigned _int64 GetFileSize(const std::wstring &filePath)
{
    HANDLE handle = ::CreateFile(filePath.c_str(), FILE_READ_EA, FILE_SHARE_READ, 0,
                                 OPEN_EXISTING, 0, 0);
    if (handle != INVALID_HANDLE_VALUE) {
        int size = ::GetFileSize(handle, NULL);
        CloseHandle(handle);
        return size;
    }
    return 0;
}

bool DeleteFile(const std::wstring &filepath)
{
    if (::DeleteFileW(filepath.c_str()) != 0)
        return true;
    return false;
}

}; // namespace ckbase
