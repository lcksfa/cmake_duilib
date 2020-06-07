#pragma once

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>
#ifdef USE_CONSOLE
// log in console
#include <spdlog/sinks/stdout_color_sinks.h>
#endif
#include "spdlog/async.h" //support for async logging
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h" // support for rotating file logging

#define BW_OUTPUT
#undef BW_OUTPUT
#define BWTRACE BW__Trace

void BW__Trace(LPCTSTR pstrFormat, ...);

namespace ckbase
{

bool SetTracingLog(const std::string &logDirectory, const std::string &logFileName);

bool SetTracingLog(const std::wstring &logDirectory, const std::string &logFileName);

inline std::string GetLogger()
{
    return "normalLogger";
}

template <typename... Args> inline void Info(const char *fmt, const Args &... args)
{
    auto logger = spdlog::get(GetLogger());
    if (logger)
        logger->info(fmt, args...);
}

template <typename... Args> inline void Info(const wchar_t *fmt, const Args &... args)
{
#ifdef BW_OUTPUT
    BW__Trace(fmt, args...);
#endif
    auto logger = spdlog::get(GetLogger());
    if (logger)
        logger->info(fmt, args...);
}

template <typename... Args> inline void Warn(const char *fmt, const Args &... args)
{
    auto logger = spdlog::get(GetLogger());
    if (logger)
        logger->warn(fmt, args...);
}

template <typename... Args> inline void Warn(const wchar_t *fmt, const Args &... args)
{
#ifdef BW_OUTPUT
    BW__Trace(fmt, args...);
#endif
    auto logger = spdlog::get(GetLogger());
    if (logger)
        logger->warn(fmt, args...);
}

} // namespace ckbase