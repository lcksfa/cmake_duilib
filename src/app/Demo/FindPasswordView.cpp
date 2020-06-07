#include "FindPasswordView.h"
#include "win32/string_code_convert.h"
#include "module\Register.h"
#include "log/logger.h"
#include "module/FindPassword.h"
#include "toast.h"

#pragma warning(disable : 4503)

FindPasswordView::FindPasswordView(
    /* args */ ui::CPaintManagerUI *ui_manager, HWND hwnd)
    : m_uiManager(ui_manager)
    , m_ownerHwnd(hwnd)
{
    m_uiManager->AddNotifier(this);
    auto FindLabelPtr = [this](const std::wstring &label) {
        return dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(label.c_str()));
    };
    m_labfindpwd_phonefillerror = FindLabelPtr(L"labfindpwd_phonefillerror");
    m_labfindpwd_verifycodefillerror = FindLabelPtr(L"labfindpwd_verifycodefillerror");
    m_labfindpwd_passwordfillerror = FindLabelPtr(L"labfindpwd_passwordfillerror");
    m_labfindpwd_passwordrecheckerror = FindLabelPtr(L"labfindpwd_passwordrecheckerror");
}

FindPasswordView::~FindPasswordView()
{
}

void FindPasswordView::Notify(ui::TNotifyUI &msg)
{
    if (msg.sType != DUI_MSGTYPE_CLICK)
        return;
    auto btnName = msg.pSender->GetName();
    if (btnName == L"btn_password_vercode") {
        DoFetchVerCode();
    }
    else if (btnName == L"btn_find_password_confirm") {
        DoPasswordChangeConform();
    }
}

void FindPasswordView::DoFetchVerCode()
{
    auto p_edit_phone_number =
        dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(L"edit_password_phoneno"));
    std::wstring phone{p_edit_phone_number->GetText().GetData()};
    if (phone.empty()) {
        ShowErrorTip(findPasswordErrorTipType::phone, L"电话号码不能为空");
        return;
    }
    Register register_logic;
    if (!register_logic.IsValidPhone(phone)) {
        ShowErrorTip(findPasswordErrorTipType::phone, L"手机号格式不正确");
        return;
    }
    auto p_btn_fetch_vercode =
        dynamic_cast<ui::CButtonUI *>(m_uiManager->FindControl(L"btn_password_vercode"));
    p_btn_fetch_vercode->SetEnabled(false);
    p_btn_fetch_vercode->SetBkColor(0xFFC4C4C4);

    auto countdownrepeattask = [=]() {
        static int countdownsecond = 60;
        std::wstring secondLeft = fmt::format(L"{}秒后重新获取", --countdownsecond);
        p_btn_fetch_vercode->SetText(secondLeft.c_str());
        if (countdownsecond == 0) {
            countdownsecond = 60;
            p_btn_fetch_vercode->SetEnabled(true);
            p_btn_fetch_vercode->SetText(L"获取短信验证码");
            p_btn_fetch_vercode->SetBkColor(0xFFFF6673);
        }
    };
    // 60秒倒计时
    PostRepeatedTask2UI(ToWeakCallback(countdownrepeattask),
                        ckbase::TimeDelta::FromMilliseconds(1000), 60);

    auto ansi_phone = ckbase::win32::UnicodeToMBCS(phone);
    HttpRequestFuncType reqVerifiCodeTask = [=]() -> UnorderedStringMap {
        FindPassword controller;
        return controller.RequestFetchVerCode(ansi_phone);
    };

    HttpResponseFuncType responseVerifiCodeTask =
        [=](const UnorderedStringMap &response) {
            if (response.at("success") == "true") {
                Toast::Show(L"验证码已发送", 2000, m_ownerHwnd);
            }
            else {
                Toast::Show(L"服务器异常,请稍候重试", 2000, m_ownerHwnd);
            }
        };

    Await(ThreadId::Misc, reqVerifiCodeTask, responseVerifiCodeTask);
}

void FindPasswordView::DoPasswordChangeConform()
{
    bool IsChecked = true;
    auto p_edit_phone_number =
        dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(L"edit_password_phoneno"));
    assert(p_edit_phone_number != nullptr);
    auto p_edit_password_pwd =
        dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(L"edit_password_pwd"));
    auto p_edit_repeat_phone_number =
        dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(L"edit_password_repeat"));
    assert(p_edit_repeat_phone_number != nullptr);
    auto phone = p_edit_phone_number->GetText();
    auto pwd1 = p_edit_password_pwd->GetText();
    auto pwd2 = p_edit_repeat_phone_number->GetText();
    if (phone.IsEmpty()) {
        IsChecked = false;
        ShowErrorTip(findPasswordErrorTipType::phone, L"电话号码不能为空");
    }
    if (pwd1.IsEmpty()) {
        IsChecked = false;
        ShowErrorTip(findPasswordErrorTipType::password, L"密码不能为空");
    }
    if (pwd1 != pwd2) {
        IsChecked = false;
        ShowErrorTip(findPasswordErrorTipType::passwordRecheck,
                     L"两次密码输入不一致，请重新输入");
    }

    auto edit_password_vercode_ =
        dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(L"edit_password_vercode"));
    assert(edit_password_vercode_ != nullptr);
    auto verifyCode = edit_password_vercode_->GetText();
    if (edit_password_vercode_->GetText().IsEmpty()) {
        IsChecked = false;
        ShowErrorTip(findPasswordErrorTipType::verificationcode, L"验证码不能为空");
    }

    auto ansi_phone = ckbase::win32::UnicodeToMBCS(phone.GetData());
    auto ansi_verify = ckbase::win32::UnicodeToMBCS(verifyCode.GetData());
    auto ansi_password = ckbase::win32::UnicodeToMBCS(pwd1.GetData());
    HttpRequestFuncType reqModiftPwdTask = [=]() -> UnorderedStringMap {
        FindPassword controller;

        return controller.RequestFindPwd(ansi_verify, ansi_phone, ansi_password);
    };

    HttpResponseFuncType responsePwdTask = [=](const UnorderedStringMap &response) {
        if (response.at("success") == "true") {
            Toast::Show(L"密码修改成功", 2000, m_ownerHwnd);
        }
        else {
            Toast::Show(L"密码修改失败，请稍后重试！", 2000, m_ownerHwnd);
        }
    };

    Await(ThreadId::Misc, reqModiftPwdTask, responsePwdTask);
}

void FindPasswordView::ShowErrorTip(findPasswordErrorTipType type,
                                    const std::wstring &message)
{
    switch (type) {

    case findPasswordErrorTipType::phone:
        m_labfindpwd_phonefillerror->SetText(message.data());
        m_labfindpwd_phonefillerror->SetVisible(true);
        break;

    case findPasswordErrorTipType::verificationcode:
        m_labfindpwd_verifycodefillerror->SetText(message.data());
        m_labfindpwd_verifycodefillerror->SetVisible(true);
        break;
    case findPasswordErrorTipType::password:
        m_labfindpwd_passwordfillerror->SetText(message.data());
        m_labfindpwd_passwordfillerror->SetVisible(true);
        break;

    case findPasswordErrorTipType::passwordRecheck:
        m_labfindpwd_passwordrecheckerror->SetText(message.data());
        m_labfindpwd_passwordrecheckerror->SetVisible(true);
        break;
    default:
        break;
    }
}

void FindPasswordView::ResetView()
{
    m_labfindpwd_phonefillerror->SetVisible(false);
    m_labfindpwd_verifycodefillerror->SetVisible(false);
    m_labfindpwd_passwordfillerror->SetVisible(false);
    m_labfindpwd_passwordrecheckerror->SetVisible(false);
}