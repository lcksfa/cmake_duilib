#ifndef __INCLUDE_WIN_UTIL_H__
#define __INCLUDE_WIN_UTIL_H__

#include "Windows.h"
#include <string>

namespace ckbase
{
namespace win32
{
std::wstring GetCurrentModuleDirectory();

bool IsAddressInCurrentModule(void *address);
bool IsModuleHandleValid(HMODULE module_handle);
HMODULE GetModuleHandleFromAddress(void *address);
HMODULE GetCurrentModuleHandle();

std::wstring GetHostName();

bool RunApp(const wchar_t *application, HANDLE *process = NULL);
bool RunAppWithCommand(const wchar_t *application, const wchar_t *command,
                       HANDLE *process = NULL);
bool RunAppWithRedirection(const wchar_t *application, const wchar_t *command,
                           HANDLE input, HANDLE output, HANDLE error,
                           HANDLE *process = NULL);

bool MinimizeProcessWorkingSize();
bool SingletonRun(const wchar_t *application);

bool IsRunningOnVistaOrHigher();

bool OpenResource(const wchar_t *resource, const wchar_t *type, void *&data,
                  unsigned long &size, HMODULE module = NULL);
std::string GetNewUuid();

bool WriteRegKey(HKEY hRoot, LPCSTR strSubKey, LPCSTR strValueName, const char *strValue,
                 int length);
bool ReadRegKey(HKEY hRoot, LPCSTR strSubKey, LPCSTR strValueName, char *strValue,
                int length);

double GetDpi();
BOOL Is64Bit_OS();

void SafeOpenUrl(const std::wstring &url, int show_cmd = SW_SHOW);

} // namespace win32
} // namespace ckbase

#endif // __INCLUDE_WIN_UTIL_H__