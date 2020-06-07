#pragma once
#include <string>
#include <map>
#include "Windows.h"

namespace ckbase
{
namespace win32
{
bool IsProcessRun(const std::wstring &processName);
DWORD GetRunProcessID(const std::wstring &strName);

bool KillProcess(const std::wstring &strName);

bool BlockExecProcess(const std::wstring &processPath, const std::wstring &cmd = L"",
                      bool IsShow = true);
bool AsyncExecProcess(const std::wstring &processPath, const std::wstring &cmd = L"",
                      bool IsShow = true);
std::map<std::wstring, int> GetServiceInfo();
bool StartWinService(const std::wstring &strServiceName);
bool StopWinService(const std::wstring &strServiceName);
} // namespace win32
} // namespace ckbase
