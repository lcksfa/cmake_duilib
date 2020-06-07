/*
 *	Copyright	WuHan, BWJF Inc.
 *	Brief		Utilities for Windows applications
 */
// #include "../pch.h"
#include <Objbase.h>
#include <assert.h>
#include "Windows.h"
#include "win32/win_util.h"
#include "fmt\format.h"

namespace ckbase
{
namespace win32
{

std::wstring GetModulePathName(HMODULE module_handle)
{
    // DCHECK(IsModuleHandleValid(module_handle));
    std::wstring mod_path;
    mod_path.resize(MAX_PATH);
    mod_path.resize(
        ::GetModuleFileNameW(module_handle, &mod_path[0], (DWORD) mod_path.size()));
    return mod_path;
}

std::wstring GetModuleDirectory(HMODULE module_handle)
{
    // DCHECK(IsModuleHandleValid(module_handle));
    std::wstring module_directory;
    // FIXME
    GetModulePathName(module_handle);
    // if (FilePathApartDirectory(GetModulePathName(module_handle), module_directory))
    //     return module_directory;
    return L"";
}

std::wstring GetCurrentModuleDirectory()
{
    auto currentModuleHandle = GetCurrentModuleHandle();

    TCHAR tszModule[MAX_PATH + 1] = {0};
    ::GetModuleFileName(currentModuleHandle, tszModule, MAX_PATH);
    std::wstring sInstancePath = tszModule;
    int pos = sInstancePath.rfind(L'\\');
    if (pos >= 0)
        sInstancePath = sInstancePath.substr(0, pos + 1);
    return sInstancePath;
}

HMODULE GetModuleHandleFromAddress(void *address)
{
    MEMORY_BASIC_INFORMATION mbi = {0};
    DWORD result = ::VirtualQuery(address, &mbi, sizeof(mbi));
    assert(result == sizeof(mbi));
    return static_cast<HMODULE>(mbi.AllocationBase);
}

HMODULE GetCurrentModuleHandle()
{
    return GetModuleHandleFromAddress(GetCurrentModuleHandle);
}

bool IsAddressInCurrentModule(void *address)
{
    return GetCurrentModuleHandle() == GetModuleHandleFromAddress(address);
}

std::wstring GetHostName()
{
    std::wstring host_name;
    DWORD name_len = MAX_COMPUTERNAME_LENGTH + 1;
    host_name.resize(name_len);
    bool result = !!::GetComputerName(&host_name[0], &name_len);
    assert(result);
    host_name.resize(name_len);
    return host_name;
}

bool IsModuleHandleValid(HMODULE module_handle)
{
    if (!module_handle)
        return true;
    return module_handle == GetModuleHandleFromAddress(module_handle);
}

bool RunApp(const wchar_t *application, HANDLE *process)
{
    return RunAppWithRedirection(application, NULL, NULL, NULL, NULL, process);
}

bool RunAppWithCommand(const wchar_t *application, const wchar_t *command,
                       HANDLE *process)
{
    return RunAppWithRedirection(application, command, NULL, NULL, NULL, process);
}

bool RunAppWithRedirection(const wchar_t *application, const wchar_t *command,
                           HANDLE input, HANDLE output, HANDLE error, HANDLE *process)
{
    PROCESS_INFORMATION pi;
    STARTUPINFOW si;

    memset(&si, 0, sizeof(si));

    if (!!input || !!output || !!error)
        si.dwFlags = STARTF_USESTDHANDLES;

    si.cb = sizeof(si);
    si.hStdInput = input ? input : ::GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = output ? output : ::GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = error ? error : ::GetStdHandle(STD_ERROR_HANDLE);

    wchar_t *command_dup = wcsdup(command);

    if (::CreateProcessW(application, command_dup, NULL, NULL,
                         (si.dwFlags & STARTF_USESTDHANDLES) ? TRUE : FALSE,
                         CREATE_NO_WINDOW | CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        ::CloseHandle(pi.hThread);
        if (process == NULL)
            ::CloseHandle(pi.hProcess);
        else
            *process = pi.hProcess;
        free(command_dup);
        return true;
    }

    free(command_dup);
    return false;
}

bool MinimizeProcessWorkingSize()
{
    OSVERSIONINFOW osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    ::GetVersionExW(&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        ::SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T) -1, (SIZE_T) -1);
        return true;
    }

    return false;
}

bool SingletonRun(const wchar_t *application)
{
    assert(application);
    if (application == NULL)
        return false;

    HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application);

    if (hMutex == NULL)
        return false;

    if (::GetLastError() == ERROR_ALREADY_EXISTS) {
        ::CloseHandle(hMutex);
        return false;
    }

    /* 从此这个互斥信号量就被不受控地打开了，直到进程退出 */
    return true;
}

bool IsRunningOnVistaOrHigher()
{
    OSVERSIONINFO os_version = {0};
    os_version.dwOSVersionInfoSize = sizeof(os_version);
    GetVersionEx(&os_version);
    return os_version.dwMajorVersion >= 6;
}

bool OpenResource(const wchar_t *resource, const wchar_t *type, void *&data,
                  unsigned long &size, HMODULE module)
{
    HRSRC resource_handle = ::FindResourceW(module, resource, type);
    if (resource_handle == NULL)
        return false;
    HGLOBAL data_handle = ::LoadResource(module, resource_handle);
    if (data_handle == NULL)
        return false;
    size = ::SizeofResource(module, resource_handle);
    if (size == 0)
        return false;
    data = ::LockResource(data_handle);
    if (data == NULL)
        return false;
    return true;
}

std::string GetNewUuid()
{
    char buf[64] = {0x00};
    GUID guid;

    if (S_OK == ::CoCreateGuid(&guid)) {
        sprintf_s(buf, sizeof(buf), "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
                  guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1],
                  guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
                  guid.Data4[6], guid.Data4[7]);
    }
    return buf;
}

bool CreateRegKey(LPCSTR strSubKey, LPCSTR strValueName, char *strValue, int length)
{
    HKEY hKey = NULL;
    // TCHAR * subKey =
    // _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\2345PCSafe\\test");
    DWORD dwOptions = REG_OPTION_NON_VOLATILE;
    DWORD dwDisposition;
    long resulte = RegCreateKeyExA(HKEY_LOCAL_MACHINE, strSubKey, 0, NULL, dwOptions,
                                   KEY_WRITE, NULL, &hKey, &dwDisposition);
    if (resulte != ERROR_SUCCESS) {
        return false;
    }
    else {
        if (dwDisposition == REG_OPENED_EXISTING_KEY) {
            return true;
        }
        else if (dwDisposition == REG_CREATED_NEW_KEY) {
            return true;
        }
    }
    return false;
}

bool WriteRegKey(HKEY hRoot, LPCSTR strSubKey, LPCSTR strValueName, const char *strValue,
                 int length)
{
    // 根键、子键名称、和到子键的句柄
    HKEY hKey;
    // 打开指定子键
    DWORD dwDisposition = REG_OPENED_EXISTING_KEY; // 如果不存在不创建
    LONG lRet = ::RegCreateKeyExA(hRoot, strSubKey, 0, NULL, REG_OPTION_NON_VOLATILE,
                                  KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (lRet != ERROR_SUCCESS)
        return false;
    // 创建一个新的键值，设置键值数据为文件名
    lRet = ::RegSetValueExA(hKey, strValueName, 0, REG_SZ, (BYTE *) strValue, length);

    ::RegCloseKey(hKey);
    if (lRet == ERROR_SUCCESS) {
        return true;
    }
    return false;
}

bool ReadRegKey(HKEY hRoot, LPCSTR strSubKey, LPCSTR strValueName, char *strValue,
                int length) //这里是传3个参数
{
    DWORD dwType = REG_SZ; //定义数据类型
    DWORD dwLen = MAX_PATH;
    char data[MAX_PATH];
    HKEY hSubKey;
    if (ERROR_SUCCESS == RegOpenKeyA(hRoot, strSubKey, &hSubKey)) {
        TCHAR buf[256] = {0};

        if (ERROR_SUCCESS ==
            RegQueryValueExA(hSubKey, strValueName, 0, &dwType, (LPBYTE) data, &dwLen)) {
            strcpy(strValue, (char *) data);
            RegCloseKey(hSubKey); //关闭注册表
            return true;
        }
    }
    RegCloseKey(hSubKey); //关闭注册表
    return false;
}
double GetDpi()
{
    // 获取窗口当前显示的监视器
    // 使用桌面的句柄.
    HWND hWnd = GetDesktopWindow();
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    // 获取监视器逻辑宽度与高度
    MONITORINFOEX miex;
    miex.cbSize = sizeof(miex);
    GetMonitorInfo(hMonitor, &miex);
    int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
    // 获取监视器物理宽度与高度
    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
    int cxPhysical = dm.dmPelsWidth;
    // 缩放比例计算  实际上使用任何一个即可
    double horzScale = ((double) cxPhysical / (double) cxLogical);
    return horzScale;
}

BOOL Is64Bit_OS()
{
    BOOL bRetVal = FALSE;
    SYSTEM_INFO si = {0};
    typedef void(WINAPI * LPFN_PGNSI)(LPSYSTEM_INFO);
    LPFN_PGNSI pGNSI = (LPFN_PGNSI) GetProcAddress(GetModuleHandle(L"kernel32.dll"),
                                                   "GetNativeSystemInfo");
    if (pGNSI == NULL) {
        return FALSE;
    }
    pGNSI(&si);
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
        si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
        bRetVal = TRUE;
    }
    else {
        // 32 位操作系统
    }
    return bRetVal;
}
void SafeOpenUrl(const std::wstring &url, int show_cmd)
{
    //暂时不做空格处理
    // std::wstring url_temp = url;
    // std::wstring url_trim = nbase::StringTrimLeft(url_temp.c_str());
    // url_trim = nbase::StringTrimRight(url_trim.c_str());
    HINSTANCE result = ::ShellExecute(NULL, L"open", url.c_str(), NULL, NULL, show_cmd);
    if ((DWORD) result <= 32) {
        ::ShellExecute(NULL, L"open", L"IEXPLORE.EXE", url.c_str(), NULL, show_cmd);
    }
}

} // namespace win32
} // namespace ckbase
