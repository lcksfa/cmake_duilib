#include "OnekeyDeclarateView.h"
#include <array>
#include <thread>
#include <chrono>
#include "toast.h"
#include "ModalWnd.hpp"
#include "closure/closure.h"
#include "async_modal/AsyncFileDialog.h"
#include "win32/string_code_convert.h"
#include "toolset/date.h"

#pragma warning(disable : 4503)

OnekeyDeclarateView::OnekeyDeclarateView(ui::CPaintManagerUI *uiManager, HWND hwnd)
    : m_uiManager(uiManager)
    , m_ownerHwnd(hwnd)
{
    m_uiManager->AddNotifier(this);

    m_comYear = dynamic_cast<ui::CComboUI *>(m_uiManager->FindControl(L"comYear"));
    assert(m_comYear != nullptr);
    InitComboYears();
    m_comYear->SelectItem(2);
    m_comSeasonOrMonth =
        dynamic_cast<ui::CComboUI *>(m_uiManager->FindControl(L"comSeasonOrMonth"));
    AddComboMonths();
    m_comSeasonOrMonth->SelectItem(0);
    m_declarTab =
        dynamic_cast<ui::CTabLayoutUI *>(m_uiManager->FindControl(L"declarTab"));
    assert(m_declarTab != nullptr);
    assert(m_comSeasonOrMonth != nullptr);

    m_lab_ok_taxno =
        dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(L"lab_ok_taxno"));

    auto FindLabelPtr = [this](const std::wstring &labelName) {
        return dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(labelName.c_str()));
    };
    m_lab_ok_3_1 = FindLabelPtr(L"lab_ok_3_1");
    m_lab_ok_3_2 = FindLabelPtr(L"lab_ok_3_2");
    m_lab_ok_3_3 = FindLabelPtr(L"lab_ok_3_3");
    m_lab_ok_3_4 = FindLabelPtr(L"lab_ok_3_4");
    m_lab_ok_6_1 = FindLabelPtr(L"lab_ok_6_1");
    m_lab_ok_6_2 = FindLabelPtr(L"lab_ok_6_2");
    m_lab_ok_query_taxer_date = FindLabelPtr(L"lab_ok_query_taxer_date");
    m_lab_ok_table_create_date = FindLabelPtr(L"lab_ok_table_create_date");
}

void OnekeyDeclarateView::SetUserTaxNo(const std::string &taxno)
{
    m_taxno = taxno;
    auto unicodeTaxno = ckbase::win32::MBCSToUnicode(taxno);
    m_lab_ok_taxno->SetText(unicodeTaxno.c_str());
}

void OnekeyDeclarateView::SetUserCompanyName(const std::string &company)
{
    m_taxerName = company;
}

void OnekeyDeclarateView::InitComboYears()
{
    std::array<std::wstring, 3> arrayYears{L"2018年", L"2019年", L"2020年"};
    for (auto &item : arrayYears) {
        ui::CListLabelElementUI *p = new ui::CListLabelElementUI();
        p->SetText(item.c_str());
        m_comYear->Add(p);
    }
}
void OnekeyDeclarateView::AddComboQuarters()
{
    std::array<std::wstring, 4> arrayQuarters{L"第一季度", L"第二季度", L"第三季度",
                                              L"第四季度"};
    m_comSeasonOrMonth->RemoveAll();
    for (auto &item : arrayQuarters) {
        ui::CListLabelElementUI *p = new ui::CListLabelElementUI();
        p->SetText(item.c_str());
        m_comSeasonOrMonth->Add(p);
    }
}

void OnekeyDeclarateView::AddComboMonths()
{
    std::array<std::wstring, 12> arrayMonths{L"1月", L"2月",  L"3月",  L"4月",
                                             L"5月", L"6月",  L"7月",  L"8月",
                                             L"9月", L"10月", L"11月", L"12月"};
    m_comSeasonOrMonth->RemoveAll();
    for (auto &item : arrayMonths) {
        ui::CListLabelElementUI *p = new ui::CListLabelElementUI();
        p->SetText(item.c_str());
        m_comSeasonOrMonth->Add(p);
    }
}

UnorderedStringMap OnekeyDeclarateView::RequestForThisYearInvoiceData()
{
    OnekeyDeclarate decl;
    auto ansiyear = ckbase::win32::UnicodeToMBCS(m_comYear->GetText().GetData());
    auto digityear = ansiyear.substr(0, 4);
    return decl.DoRequestForYearInvoices(m_taxno, digityear);
}

UnorderedStringMap OnekeyDeclarateView::RequestForMonthInvoiceData()
{
    OnekeyDeclarate decl;
    auto ansiyear = ckbase::win32::UnicodeToMBCS(m_comYear->GetText().GetData());
    auto digityear = ansiyear.substr(0, 4);
    auto monthIndex = m_comSeasonOrMonth->GetCurSel() + 1;

    return decl.DoRequestForMonthInvoices(m_taxno, digityear, monthIndex);
}

UnorderedStringMap OnekeyDeclarateView::RequestForQuarterInvoiceData()
{
    OnekeyDeclarate decl;
    auto ansiyear = ckbase::win32::UnicodeToMBCS(m_comYear->GetText().GetData());
    auto digityear = ansiyear.substr(0, 4);
    auto quarterIndex = m_comSeasonOrMonth->GetCurSel() + 1;

    return decl.DoRequestForQuarterInvoices(m_taxno, digityear, quarterIndex);
}

void OnekeyDeclarateView::OnClickQuery()
{
    ModalWnd::ShowWaiting(m_ownerHwnd);

    //查找全年的
    HttpRequestFuncType reqThisYearInvoiceDataTask = [=]() {
        return RequestForThisYearInvoiceData();
    };

    HttpResponseFuncType responseYearRet = [=](const UnorderedStringMap &response) {
        //填充全年

        auto todayDate = ckbase::datetime::getCurrentDate();
        auto unicodeToday = ckbase::win32::MBCSToUnicode(todayDate);
        m_lab_ok_table_create_date->SetText(unicodeToday.c_str());
        if (response.at("success") == "true") {
            auto unicodelabor_rate_3_je =
                ckbase::win32::MBCSToUnicode(response.at("labor_rate_3_je"));
            m_lab_ok_3_3->SetText(unicodelabor_rate_3_je.substr(0, 4).c_str());
            auto unicodeasserts_rate_3_je =
                ckbase::win32::MBCSToUnicode(response.at("asserts_rate_3_je"));
            m_lab_ok_3_4->SetText(unicodeasserts_rate_3_je.substr(0, 4).c_str());
            auto unicodeasserts_rate_5_je =
                ckbase::win32::MBCSToUnicode(response.at("asserts_rate_5_je"));
            m_lab_ok_6_2->SetText(unicodeasserts_rate_5_je.substr(0, 4).c_str());
        }
        else {
            m_lab_ok_3_3->SetText(L"0.00");
            m_lab_ok_3_4->SetText(L"0.00");
            m_lab_ok_6_2->SetText(L"0.00");
        }
    };

    auto _optByMonth =
        dynamic_cast<ui::COptionUI *>(m_uiManager->FindControl(L"optByMonth"));
    bool isSelectMonth = _optByMonth->IsSelected();

    HttpRequestFuncType reqMonthOrQuarter = [=]() {
        if (isSelectMonth)
            return RequestForMonthInvoiceData();
        else
            return RequestForQuarterInvoiceData();
    };

    HttpResponseFuncType responseMonthOrQuarterRet =
        [=](const UnorderedStringMap &response) {
            ModalWnd::CloseModal();
            //填充当选日期
            auto dateSpaining = response.at("startDate") + " ~ " + response.at("endDate");
            auto unicodeDateString = ckbase::win32::MBCSToUnicode(dateSpaining);
            m_lab_ok_query_taxer_date->SetText(unicodeDateString.c_str());

            if (response.at("success") == "true") {
                auto unicodelabor_rate_3_je =
                    ckbase::win32::MBCSToUnicode(response.at("labor_rate_3_je"));
                auto unicodeasserts_rate_3_je =
                    ckbase::win32::MBCSToUnicode(response.at("asserts_rate_3_je"));
                auto unicodeasserts_rate_5_je =
                    ckbase::win32::MBCSToUnicode(response.at("asserts_rate_5_je"));
                m_lab_ok_3_1->SetText(unicodelabor_rate_3_je.substr(0, 4).c_str());
                m_lab_ok_3_2->SetText(unicodeasserts_rate_3_je.substr(0, 4).c_str());
                m_lab_ok_6_1->SetText(unicodeasserts_rate_5_je.substr(0, 4).c_str());
            }
            else {
                m_lab_ok_3_1->SetText(L"0.00");
                m_lab_ok_3_2->SetText(L"0.00");
                m_lab_ok_6_1->SetText(L"0.00");
            }
        };
    Await(ThreadId::Misc, reqThisYearInvoiceDataTask, responseYearRet);
    Await(ThreadId::Misc, reqMonthOrQuarter, responseMonthOrQuarterRet);
}

void OnekeyDeclarateView::OnSelectOutputExeclPath(BOOL IsRetSuccess,
                                                  const std::wstring &filePath)
{
    if (!IsRetSuccess)
        return;

    std::function<bool(void)> outoutexeclTask = [=]() {
        OnekeyDeclarate decl;
        auto inputParm1 = m_lab_ok_3_1->GetText().GetData();
        auto inputParm2 = m_lab_ok_3_2->GetText().GetData();
        auto inputParm3 = m_lab_ok_3_3->GetText().GetData();
        auto inputParm4 = m_lab_ok_3_4->GetText().GetData();
        auto inputParm5 = m_lab_ok_6_1->GetText().GetData();
        auto inputParm6 = m_lab_ok_6_2->GetText().GetData();

        auto unicodeTaxerName = ckbase::win32::MBCSToUnicode(m_taxerName);
        auto wstrFillFormDate = m_lab_ok_table_create_date->GetText().GetData();
        auto wstrTaxPayDate = m_lab_ok_query_taxer_date->GetText().GetData();
        auto unicodeTaxno = ckbase::win32::MBCSToUnicode(m_taxno);
        return decl.FillOutputExcel(
            filePath, unicodeTaxerName, wstrTaxPayDate, wstrFillFormDate, unicodeTaxno,
            inputParm1, inputParm2, inputParm3, inputParm4, inputParm5, inputParm6);
    };

    std::function<void(bool)> resultReply = [=](bool outputSuccess) {
        if (outputSuccess)
            Toast::Show(L"导出成功", 2000, m_ownerHwnd);
        else
            Toast::Show(L"导出失败", 2000, m_ownerHwnd);
    };
    Await(ThreadId::Misc, outoutexeclTask, resultReply);
}

void OnekeyDeclarateView::DoOutputExcel()
{
    auto *file_dialog = new AsyncFileDialog;

    std::map<LPCTSTR, LPCTSTR> filters;
    filters[L"Execl文件(*.xls)"] = L"*.xls";

    file_dialog->SetFilter(filters);
    file_dialog->SetParentWnd(m_ownerHwnd);

    AsyncFileDialog::FileDialogCallback callback =
        std::bind(&OnekeyDeclarateView::OnSelectOutputExeclPath, this,
                  std::placeholders::_1, std::placeholders::_2);

    file_dialog->AsyncShowSaveFileDlg(callback);
}

void OnekeyDeclarateView::Notify(ui::TNotifyUI &msg)
{
    auto ctrlName = msg.pSender->GetName();

    if (msg.sType == DUI_MSGTYPE_CLICK) {
        OnBtnClick(ctrlName.GetData());
    }
    else if (msg.sType == DUI_MSGTYPE_PREDROPDOWN) {
    }
    else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED) {
        if (ctrlName == L"optByMonth") {
            AddComboMonths();
            m_comSeasonOrMonth->SelectItem(0);
        }
        else if (ctrlName == L"optByQuarter") {
            AddComboQuarters();
            m_comSeasonOrMonth->SelectItem(0);
        }
    }
}

void OnekeyDeclarateView::OnBtnClick(const std::wstring &btnName)
{
    if (btnName == L"btn_declarate_query") {
        OnClickQuery();
    }
    else if (btnName == L"btn_declarate_excel_output") {
        DoOutputExcel();
    }
    else if (btnName == L"btn_declare_online") {
        ModalWnd::ShowWaiting(m_ownerHwnd);
        std::function<void(void)> decl = [this]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        };
        std::function<void(void)> respReply = [this]() {
            ModalWnd::CloseModal();
            m_declarTab->SelectItem(1);
        };
        Await(ThreadId::Misc, decl, respReply);
    }
    else if (btnName == L"btn_continue_declarate") {
        m_declarTab->SelectItem(0);
    }
}
