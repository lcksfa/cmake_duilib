#pragma once
#include <chrono>
#include <string>
namespace ckbase
{
namespace datetime
{
std::string getFormatTimeStamp();
std::string getCurrentDate();
std::string getNoneFormatTimeStamp();
std::string getFormatDateWithConnector(const std::string &Connector = "-");
std::string getQuarterFirstDate(int quarterIndex);
std::string getQuarterLastDate(int quarterIndex);
class DateFormatter
{
private:
    uint16_t year_;
    short month_;
    short day_;

public:
    DateFormatter(uint16_t year, short month, short day);
    DateFormatter(const std::string &strDate);
    DateFormatter(const std::string &year, const std::string &month,
                  const std::string &day);
    std::string getDateString();
    short getDaysOfMonth();
    std::string nextDayDate();
    std::string nextMonthDate();

private:
    bool isLeapYear();
};



} // namespace datetime
} // namespace ckbase
