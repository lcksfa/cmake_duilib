// #include "../pch.h"
#include <string>
#include "Windows.h"
#include "shellapi.h"
#include "app_env\app_ins.h"
#include "win32\win_util.h"

std::wstring AppEnvironment::GetRunningPath()
{
    return ckbase::win32::GetCurrentModuleDirectory();
}

void AppEnvironment::AddNewEnvironment(const std::wstring &directory)
{
    TCHAR path_environment[4096];
    GetEnvironmentVariable(L"PATH", path_environment, 4096);
    std::wstring new_path = directory + L";";
    std::wstring new_environment = new_path + path_environment;
    SetEnvironmentVariable(L"PATH", new_environment.c_str());
}
