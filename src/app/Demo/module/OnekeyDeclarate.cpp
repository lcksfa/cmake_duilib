#include "OnekeyDeclarate.h"
#include "app_env/app_ins.h"
#include "toolset/date.h"

using namespace libxl;

bool OnekeyDeclarate::FillOutputExcel(
    const std::wstring &savePath, const std::wstring &taxername,
    const std::wstring &wstrTaxPayDate, const std::wstring &wstrFillFormDate,
    const std::wstring &taxno, const std::wstring &inputParm1,
    const std::wstring &inputParm2, const std::wstring &inputParm3,
    const std::wstring &inputParm4, const std::wstring &inputParm5,
    const std::wstring &inputParm6)
{
    auto *book = xlCreateBook();

    // Execl table path
    auto apprunnerPath = AppEnvironment::GetRunningPath();
    auto modelExeclPath = apprunnerPath + L"model\\onekeymodel.xls";

    try {
        if (book->load(modelExeclPath.c_str())) {
            FillData(book, taxername, wstrTaxPayDate, wstrFillFormDate, taxno, inputParm1,
                     inputParm2, inputParm3, inputParm4, inputParm5, inputParm6);
        }
        book->save(savePath.c_str());
    }
    catch (...) {
        throw book->errorMessage();
        return false;
    }
    return true;
}

Font *OnekeyDeclarate::MakeFont(Book *book, const wstring &FontName, int fontSize)
{
    Font *FontMake = book->addFont();
    FontMake->setName(FontName.c_str());
    FontMake->setSize(fontSize);

    return FontMake;
}

libxl::Format *OnekeyDeclarate::SetNumberFormat(libxl::Book *book)
{
    Format *numberFormat = book->addFormat();
    numberFormat->setFont(MakeFont(book, L"Times New Roman", 11));
    numberFormat->setAlignH(ALIGNH_RIGHT);
    numberFormat->setAlignV(ALIGNV_CENTER);
    numberFormat->setBorder(BORDERSTYLE_THIN);
    return numberFormat;
}

libxl::Format *OnekeyDeclarate::SetStringFormat(libxl::Book *book)
{
    Format *stringFormat = book->addFormat();
    stringFormat->setFont(MakeFont(book, L"宋体", 10));
    stringFormat->setAlignH(ALIGNH_LEFT);
    stringFormat->setAlignV(ALIGNV_CENTER);
    // stringFormat->setBorder(BORDERSTYLE_THIN);

    return stringFormat;
}

void OnekeyDeclarate::FillData(
    libxl::Book *book, const std::wstring &taxername, const std::wstring &wstrTaxPayDate,
    const std::wstring &wstrFillFormDate, const std::wstring &taxno,
    const std::wstring &inputParm1, const std::wstring &inputParm2,
    const std::wstring &inputParm3, const std::wstring &inputParm4,
    const std::wstring &inputParm5, const std::wstring &inputParm6)
{
    libxl::Format *p_numberFormat = SetNumberFormat(book);
    Format *p_stringFormat = SetStringFormat(book);
    Sheet *sheet = book->getSheet(0);
    if (!sheet)
        return;
    //表头信息填充
    wstring innerwstrNsrsbh(L"纳税人识别号：");
    wstring innerwstrTaxPayDate(L"税款所属期：");
    wstring innerwstrFillFormDate(L"填表日期：");

    innerwstrNsrsbh += taxno;
    innerwstrTaxPayDate += wstrTaxPayDate;
    innerwstrFillFormDate += wstrFillFormDate;
    sheet->writeStr(2, 0, innerwstrNsrsbh.c_str(), p_stringFormat);
    sheet->writeStr(4, 0, innerwstrTaxPayDate.c_str(), p_stringFormat);
    sheet->writeStr(4, 5, innerwstrFillFormDate.c_str(), p_stringFormat);
    // the string from calc result
    //向单元格填入计算数据
    sheet->writeStr(11, 4, inputParm1.c_str(), p_numberFormat);
    sheet->writeStr(11, 5, inputParm2.c_str(), p_numberFormat);
    sheet->writeStr(15, 5, inputParm3.c_str(), p_numberFormat);

    sheet->writeStr(11, 6, inputParm4.c_str(), p_numberFormat);
    sheet->writeStr(11, 7, inputParm5.c_str(), p_numberFormat);
    sheet->writeStr(15, 7, inputParm6.c_str(), p_numberFormat);
}

UnorderedStringMap OnekeyDeclarate::DoRequestForYearInvoices(const std::string &taxNo,
                                                             const std::string year)
{
    GetInvoicesRequest requestInstance;
    auto startDate = year + "-01-01";
    auto endDate = year + "-12-31";
    auto params = requestInstance.MakeRequestParams(taxNo, startDate, endDate);
    auto httpResponse = requestInstance.request(params);
    httpResponse["startDate"] = startDate;
    httpResponse["endDate"] = endDate;

    return httpResponse;
}

UnorderedStringMap OnekeyDeclarate::DoRequestForMonthInvoices(const std::string &taxNo,
                                                              const std::string &year,
                                                              int WithMonth)
{
    auto month = std::to_string(WithMonth);
    ckbase::datetime::DateFormatter dataformatter(year, month, "01");
    auto monthDaysCount = dataformatter.getDaysOfMonth();
    ckbase::datetime::DateFormatter dateFormatterend(year, month,
                                                     std::to_string(monthDaysCount));

    GetInvoicesRequest requestInstance;
    auto startDate = dataformatter.getDateString();
    auto endDate = dateFormatterend.getDateString();
    auto params = requestInstance.MakeRequestParams(taxNo, startDate, endDate);
    auto httpResponse = requestInstance.request(params);
    httpResponse["startDate"] = startDate;
    httpResponse["endDate"] = endDate;

    return httpResponse;
}

UnorderedStringMap OnekeyDeclarate::DoRequestForQuarterInvoices(const std::string &taxNo,
                                                                const std::string &year,
                                                                int WithQuarter)
{
    GetInvoicesRequest requestInstance;
    auto startDate = year + "-" + ckbase::datetime::getQuarterFirstDate(WithQuarter);
    auto endDate = year + "-" + ckbase::datetime::getQuarterLastDate(WithQuarter);
    auto params = requestInstance.MakeRequestParams(taxNo, startDate, endDate);
    auto httpResponse = requestInstance.request(params);
    httpResponse["startDate"] = startDate;
    httpResponse["endDate"] = endDate;

    return httpResponse;
}
