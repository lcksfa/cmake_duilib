#ifndef __INCLUDE_ONEKEYDECLARATE_H__
#define __INCLUDE_ONEKEYDECLARATE_H__
#include "libxl/libxl.h"
#include "WPHttpRequest.h"

#pragma once
#include <string>
/**
 * @brief 一键申报数据计算类
 * @param[in]
 * @return
 */
class OnekeyDeclarate
{
public:
    OnekeyDeclarate() = default;
    ~OnekeyDeclarate() = default;

    bool FillOutputExcel(const std::wstring &savePath, const std::wstring &taxername,
                         const std::wstring &wstrTaxPayDate,
                         const std::wstring &wstrFillFormDate, const std::wstring &taxno,
                         const std::wstring &inputParm1, const std::wstring &inputParm2,
                         const std::wstring &inputParm3, const std::wstring &inputParm4,
                         const std::wstring &inputParm5,
                         const std::wstring &inputParm6) noexcept(false);

    /**
     * @brief 从后台获取全年发票信息，这里有点憨憨，无奈后台不愿意算好了传回
     * @param[in]
     * @return
     */
    UnorderedStringMap DoRequestForYearInvoices(const std::string &taxNo,
                                                const std::string year);
    UnorderedStringMap DoRequestForMonthInvoices(const std::string &taxNo,
                                                 const std::string &year, int monthIndex);
    UnorderedStringMap DoRequestForQuarterInvoices(const std::string &taxNo,
                                                   const std::string &year,
                                                   int WithQuarte);

private:
    void FillData(libxl::Book *book, const std::wstring &taxername,
                  const std::wstring &wstrTaxPayDate,
                  const std::wstring &wstrFillFormDate, const std::wstring &taxno,
                  const std::wstring &inputParm1, const std::wstring &inputParm2,
                  const std::wstring &inputParm3, const std::wstring &inputParm4,
                  const std::wstring &inputParm5, const std::wstring &inputParm6);

    libxl::Format *SetNumberFormat(libxl::Book *book);
    libxl::Format *OnekeyDeclarate::SetStringFormat(libxl::Book *book);
    libxl::Font *MakeFont(libxl::Book *book, const wstring &FontName, int fontSize);
};
#endif // __INCLUDE_ONEKEYDECLARATE_H__