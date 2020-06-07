#include "win32/process_operator.h"
#include <tlhelp32.h>
#include <shellapi.h>

namespace ckbase
{
namespace win32
{
bool IsProcessRun(const std::wstring &processName)
{
    bool bFound = false;
    HANDLE hSnapshot;
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    Process32First(hSnapshot, &pe);
    do {
        std::wstring wstrTmp = pe.szExeFile;
        if (std::string::npos != wstrTmp.find(processName)) {
            bFound = true;
            break;
        }
    } while (Process32Next(hSnapshot, &pe));
    CloseHandle(hSnapshot);

    return bFound;
}

DWORD GetRunProcessID(const std::wstring &strName)
{
    DWORD dwProcessID = 0;
    HANDLE hSnapshot;
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(pe);
        Process32First(hSnapshot, &pe);
        do {
            if (wcscmp(pe.szExeFile, strName.c_str()) == 0) {
                dwProcessID = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
        CloseHandle(hSnapshot);
    }
    return dwProcessID;
}

bool StartProcess(const std::wstring &processPath, const std::wstring &strCmdLine,
                  bool bWaitForever, bool IsShow)
{
    SHELLEXECUTEINFO ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = processPath.c_str();
    ShExecInfo.lpParameters = strCmdLine.c_str();
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = IsShow ? SW_SHOW : SW_HIDE;
    ShExecInfo.hInstApp = NULL;
    BOOL executeRet = ShellExecuteEx(&ShExecInfo);
    if (executeRet != FALSE) {
        if (bWaitForever)
            WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
        return true;
    }
    return false;
}

bool BlockExecProcess(const std::wstring &processPath, const std::wstring &cmd,
                      bool IsShow)
{
    return StartProcess(processPath, cmd, true, IsShow);
}

bool AsyncExecProcess(const std::wstring &processPath, const std::wstring &cmd,
                      bool IsShow)
{
    return StartProcess(processPath, cmd, false, IsShow);
}

bool KillProcess(const std::wstring &processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return false;

    bool bFound = false;
    BOOL bRet = FALSE;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    Process32First(hSnapshot, &pe);
    do {
        std::wstring findedPeName(pe.szExeFile);
        if (findedPeName.find(processName) == std::string::npos)
            continue;
        bFound = true;
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, TRUE, pe.th32ProcessID);
        if (hProcess) {
            bRet = TerminateProcess(hProcess, 0);
            DWORD waitRet = WaitForSingleObject(hProcess, INFINITE);
            if (waitRet == WAIT_OBJECT_0)
                break;
        }
    } while (Process32Next(hSnapshot, &pe));
    CloseHandle(hSnapshot);

    if (!bFound)
        return true;

    return bRet != FALSE;
}

std::map<std::wstring, int> GetServiceInfo()
{
    std::map<std::wstring, int> servicesMap;
    SC_HANDLE SCMan =
        OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE); //打开系统服务控制器
    if (SCMan == NULL) {
        return servicesMap;
    }
    LPENUM_SERVICE_STATUS service_status = NULL; //保存系统服务的结构
    DWORD cbBytesNeeded = NULL;
    DWORD ServicesReturned = NULL;
    DWORD ResumeHandle = NULL;

    //获取系统服务的简单信息
    BOOL ESS = EnumServicesStatus(
        SCMan,                                  //系统服务句柄
        SERVICE_DRIVER | SERVICE_WIN32,         //服务的类型
        SERVICE_STATE_ALL,                      //服务的状态
        (LPENUM_SERVICE_STATUS) service_status, //输出参数，系统服务的结构
        0,                                      // 结构的大小
        &cbBytesNeeded,    //输出参数，接收返回所需的服务
        &ServicesReturned, //输出参数，接收返回服务的数量
        &ResumeHandle); //输入输出参数，第一次调用必须为0，返回为0代表成功

    if (ESS) {
        return servicesMap;
    }

    service_status = (LPENUM_SERVICE_STATUS) new char[cbBytesNeeded];
    ZeroMemory(service_status, cbBytesNeeded);
    ESS = EnumServicesStatus(
        SCMan,                                  //系统服务句柄
        SERVICE_WIN32,                          //服务的类型
        SERVICE_STATE_ALL,                      //服务的状态
        (LPENUM_SERVICE_STATUS) service_status, //输出参数，系统服务的结构
        cbBytesNeeded,                          // 结构的大小
        &cbBytesNeeded,    //输出参数，接收返回所需的服务
        &ServicesReturned, //输出参数，接收返回服务的数量
        &ResumeHandle);

    if (ESS == false) {
        return servicesMap;
    }

    for (DWORD i = 0; i < ServicesReturned; i++) {
        std::wstring strServiceName = service_status[i].lpDisplayName;
        if (strServiceName.length()) {
            servicesMap[strServiceName] = service_status[i].ServiceStatus.dwCurrentState;
        }
    }
    CloseServiceHandle(SCMan); //关闭服务管理器的句柄
    delete service_status;
    return servicesMap;
}

bool StartWinService(const std::wstring &strServiceName)
{
    // 打开服务管理对象
    SC_HANDLE hSC = ::OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
    if (hSC == NULL) {
        return false;
    }
    // 打开www服务。
    SC_HANDLE hSvc = ::OpenService(hSC, strServiceName.c_str(),
                                   SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if (hSvc == NULL) {
        //        DWORD dwErr = GetLastError();
        ::CloseServiceHandle(hSC);
        return false;
    }
    // 获得服务的状态
    SERVICE_STATUS status;
    if (::QueryServiceStatus(hSvc, &status) == FALSE) {
        ::CloseServiceHandle(hSvc);
        ::CloseServiceHandle(hSC);
        return false;
    }
    if (status.dwCurrentState != SERVICE_RUNNING) {
        // 启动服务
        if (::StartService(hSvc, NULL, NULL) == FALSE) {
            ::CloseServiceHandle(hSvc);
            ::CloseServiceHandle(hSC);
            return false;
        }
        // 等待服务启动
        while (::QueryServiceStatus(hSvc, &status) == TRUE) {
            ::Sleep(status.dwWaitHint);
            if (status.dwCurrentState == SERVICE_RUNNING) {
                ::CloseServiceHandle(hSvc);
                ::CloseServiceHandle(hSC);
                return true;
            }
        }
    }
    ::CloseServiceHandle(hSvc);
    ::CloseServiceHandle(hSC);
    return false;
}

bool StopWinService(const std::wstring &strServiceName)
{
    SC_HANDLE schService = NULL;
    SC_HANDLE SchSCManager = NULL;
    SERVICE_STATUS serviceStatus;
    bool bRet = false;

    SchSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (SchSCManager == NULL) {
        goto EXIT;
    }
    schService = OpenService(SchSCManager, strServiceName.c_str(), SERVICE_ALL_ACCESS);
    if (schService == NULL) {
        goto EXIT;
    }
    if (ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus)) {
        bRet = true;
    }
    // 等待服务关闭
    SERVICE_STATUS status;
    while (::QueryServiceStatus(schService, &status) == TRUE) {
        ::Sleep(status.dwWaitHint);
        if (status.dwCurrentState == SERVICE_STOPPED) {
            break;
        }
    }

EXIT:
    if (schService) {
        CloseServiceHandle(schService);
    }
    if (SchSCManager) {
        CloseServiceHandle(SchSCManager);
    }
    return bRet;
}

} // namespace win32
} // namespace ckbase
