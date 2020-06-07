// #include "../pch.h"
#include <vector>
#include <string>
#include "windows.h"
#include "tchar.h"
#include "win32\string_code_convert.h"
#include "log\logger.h"

#define BUFFCOUNT (2048)
void debugLogPrint(LPCTSTR pstrFormat, va_list argp)
{
    int nSize = 0;
    wchar_t szBuffer[BUFFCOUNT];
    memset(szBuffer, 0, sizeof(szBuffer));

    nSize = _vsnwprintf_s(szBuffer, BUFFCOUNT - 1, _TRUNCATE, pstrFormat, argp); // C4996

    std::wstring strMsg = _T("BWDEBUG:");
    strMsg += szBuffer;
    strMsg += _T("\n");
    OutputDebugString(strMsg.c_str());
}
void BW__Trace(LPCTSTR pstrFormat, ...)
{
    va_list argp;
    va_start(argp, pstrFormat);
    debugLogPrint(pstrFormat, argp);
    va_end(argp);
}

namespace ckbase
{

bool SetTracingLog(const std::wstring &logDirectory, const std::string &logFileName)
{
    // the directory must be existing
    // if (!bwbase::IsDirectoryComponent(logDirectory))
    //     return false;

    std::string filename = ckbase::win32::UnicodeToMBCS(logDirectory) + logFileName;
    std::string pattern = "%^[%L][%D %H:%M:%S %e][TID:%5t]%$: %v ";
    try {
        std::vector<spdlog::sink_ptr> sinks;
#ifdef USE_CONSOLE
        AllocConsole();
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern(pattern);
        sinks.push_back(console_sink);
#endif
        spdlog::init_thread_pool(10000, 1);
        // create 3 rotating file ,each one max size is 10M.
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            filename, 1024 * 1024 * 10, 3);
        rotating_sink->set_pattern(pattern);
        sinks.push_back(rotating_sink);
        auto logger = std::make_shared<spdlog::async_logger>(
            GetLogger(), sinks.begin(), sinks.end(), spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);
        spdlog::register_logger(logger);
        spdlog::flush_every(std::chrono::seconds(3));
    }

    catch (const spdlog::spdlog_ex &) {
        return false;
    }
    return true;
}

} // namespace ckbase