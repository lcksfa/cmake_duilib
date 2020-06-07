#ifndef __INCLUDE_FINDPASSWORDVIEW_H__
#define __INCLUDE_FINDPASSWORDVIEW_H__

#pragma once
#include <string>
#include "UIlib.h"
#include "closure\closure.h"

enum class findPasswordErrorTipType
{
    phone,
    verificationcode,
    password,
    passwordRecheck
};

class FindPasswordView : public ui::INotifyUI, virtual public ckbase::SupportWeakCallback
{
private:
    /* data */
    ui::CPaintManagerUI *m_uiManager;
    ui::CLabelUI *m_labfindpwd_phonefillerror = nullptr;
    ui::CLabelUI *m_labfindpwd_verifycodefillerror = nullptr;
    ui::CLabelUI *m_labfindpwd_passwordfillerror = nullptr;
    ui::CLabelUI *m_labfindpwd_passwordrecheckerror = nullptr;

    HWND m_ownerHwnd;

public:
    FindPasswordView(/* args */ ui::CPaintManagerUI *ui_manager,HWND hwnd);
    ~FindPasswordView();
    void ResetView();

private:
    void Notify(ui::TNotifyUI &msg);

    void DoFetchVerCode();
    void DoPasswordChangeConform();

    void ShowErrorTip(findPasswordErrorTipType type, const std::wstring &message);
};

#endif // __INCLUDE_FINDPASSWORDVIEW_H__