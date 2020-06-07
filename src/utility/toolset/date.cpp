// #include "../pch.h"
#include "toolset/date.h"
#include <ctime>
#include "toolset/string_util.h"
#include <chrono>
#include <string>
#include <vector>
namespace ckbase
{
namespace datetime
{

static const int DaysInMonth[] = {31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

std::time_t getNoneFormattedNowTimeStamp()
{
    auto newtimepoint = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(newtimepoint);
}

struct tm getLocalTimeStruct()
{
    auto time_tTime = getNoneFormattedNowTimeStamp();
    struct tm buf;
    char timedisplay[100] = {0x00};
    localtime_s(&buf, &time_tTime);
    // FIXme
    return std::move(buf);
}

std::string getNoneFormatTimeStamp()
{
    auto time_tTime = getNoneFormattedNowTimeStamp();
    return std::to_string(time_tTime);
}

std::string getFormatTimeStamp()
{
    char timedisplay[100] = {0x00};
    auto structTimeBuf = getLocalTimeStruct();
    if (std::strftime(timedisplay, sizeof(timedisplay), "%F %T", &structTimeBuf)) {
        return std::string(timedisplay);
    }
    return std::string();
}

std::string getFormatDateWithConnector(const std::string &Connector)
{
    char timedisplay[100] = {0x00};
    auto structTimeBuf = getLocalTimeStruct();
    if (std::strftime(timedisplay, sizeof(timedisplay), "%Y-%m-%d", &structTimeBuf)) {
        std::string output(timedisplay);
        if (Connector != "-")
            ckbase::StringReplaceAll(std::string("-"), std::string(Connector), output);
        return output;
    }
    return std::string();
}

std::string getCurrentDate()
{
    return getFormatDateWithConnector();
}

std::string getQuarterLastDate(int quarterIndex)
{
    static std::vector<std::string> quarterMonth;
    quarterMonth.push_back("03-31");
    quarterMonth.push_back("06-30");
    quarterMonth.push_back("09-30");
    quarterMonth.push_back("12-31");
    return quarterMonth[quarterIndex - 1];
}

std::string getQuarterFirstDate(int quarterIndex)
{
    static std::vector<std::string> quarterMonth;
    quarterMonth.push_back("01-01");
    quarterMonth.push_back("04-01");
    quarterMonth.push_back("07-01");
    quarterMonth.push_back("10-01");
    return quarterMonth[quarterIndex - 1];
}

DateFormatter::DateFormatter(uint16_t year, short month, short day)
    : year_(year)
    , month_(month)
    , day_(day)
{
}

DateFormatter::DateFormatter(const std::string &strDate)
{

    if (strDate.length() != 8)
        return;
    year_ = atoi(strDate.substr(0, 4).c_str());
    month_ = atoi(strDate.substr(4, 2).c_str());
    day_ = atoi(strDate.substr(6, 2).c_str());
}

DateFormatter::DateFormatter(const std::string &year, const std::string &month,
                             const std::string &day)
    : year_(std::stoi(year))
    , month_(std::stoi(month))
    , day_(std::stoi(day))
{
}

std::string DateFormatter::getDateString()
{
    char szTmp[80] = {0x00};
    sprintf_s(szTmp, sizeof(szTmp), "%04d-%02d-%02d", year_, month_, day_);
    return std::string(szTmp);
}

bool DateFormatter::isLeapYear()
{
    if (year_ % 4 == 0) {
        if (year_ % 100 == 0) {
            if (year_ % 400 == 0)
                return true;
            return false;
        }
        return true;
    }
    return false;
}
short DateFormatter::getDaysOfMonth()
{
    if (month_ == 2) {
        if (isLeapYear())
            return 29;
        else
            return 28;
    }
    else
        return DaysInMonth[month_ - 1];
}

std::string DateFormatter::nextMonthDate()
{
    if (month_++ == 12) {
        month_ = 1;
        year_++;
    }

    return getDateString();
}

std::string DateFormatter::nextDayDate()
{
    int DaysInMonthLocal[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (isLeapYear())
        DaysInMonthLocal[1] = 29;

    if (day_ == DaysInMonthLocal[month_ - 1]) {
        day_ = 1;

        // last day of year
        if (month_++ == 12) {
            year_++;
            month_ = 1;
        }
    }
    else {
        day_++;
    }
    return getDateString();
}
} // namespace datetime

} // namespace ckbase
