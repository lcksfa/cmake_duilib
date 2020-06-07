#ifndef __INCLUDE_ONEKEYDECLARATEVIEW_H__
#define __INCLUDE_ONEKEYDECLARATEVIEW_H__

#pragma once
#include "UIlib.h"
#include <string>
#include "module/OnekeyDeclarate.h"

class OnekeyDeclarateView : public ui::INotifyUI
{
public:
    OnekeyDeclarateView(ui::CPaintManagerUI *uiManager, HWND hwnd);
    ~OnekeyDeclarateView() = default;

public:
    void SetUserTaxNo(const std::string &taxno);
    void SetUserCompanyName(const std::string &company);

private:
    void Notify(ui::TNotifyUI &msg);

    void OnBtnClick(const std::wstring &btnName);

    void InitComboYears();
    void AddComboQuarters();
    void AddComboMonths();

    void OnClickQuery();

    void DoOutputExcel();

    void OnSelectOutputExeclPath(BOOL IsRetSuccess, const std::wstring &filePath);

    UnorderedStringMap RequestForThisYearInvoiceData();
    UnorderedStringMap RequestForMonthInvoiceData();
    UnorderedStringMap RequestForQuarterInvoiceData();

private:
    ui::CPaintManagerUI *m_uiManager;
    HWND m_ownerHwnd;
    std::string m_taxno;
    std::string m_taxerName;

    ui::CComboUI *m_comYear = nullptr;
    ui::CComboUI *m_comSeasonOrMonth = nullptr;
    ui::CTabLayoutUI *m_declarTab = nullptr;

    ui::CLabelUI *m_lab_ok_taxno = nullptr;
    ui::CLabelUI *m_lab_ok_query_taxer_date = nullptr;
    ui::CLabelUI *m_lab_ok_table_create_date = nullptr;
    ui::CLabelUI *m_lab_ok_3_1 = nullptr;
    ui::CLabelUI *m_lab_ok_3_2 = nullptr;
    ui::CLabelUI *m_lab_ok_3_3 = nullptr;
    ui::CLabelUI *m_lab_ok_3_4 = nullptr;
    ui::CLabelUI *m_lab_ok_6_1 = nullptr;
    ui::CLabelUI *m_lab_ok_6_2 = nullptr;
};

#endif // __INCLUDE_ONEKEYDECLARATEVIEW_H__