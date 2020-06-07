/*
 * Written by Li Zhao <bw_lizhao AT whty Dot com Dot cn>.
 * On 2020年5月28日
 * This file use for Login View
 *
 */
#include "pch.h"
#include "LoginView.h"
#include "ModalWnd.hpp"
#include "closure/closure.h"
#include "win32/string_code_convert.h"
#include "toast.h"
#include "module/InternalDB.h" //for AccountInfo

// I don't know why when I use function callable object ,VS2015 complie this warning ,so
// disable it first
#pragma warning(disable : 4503)

LoginView::LoginView(ui::CPaintManagerUI *uiManager, HWND hwnd)
    : m_uiManager(uiManager)
    , m_ownerHwnd(hwnd)
    , m_logic_login(std::make_unique<Login>())
    , m_current_user_info(std::make_unique<UserInfoView>())
{
    m_uiManager->AddNotifier(this);
}

void LoginView::Notify(ui::TNotifyUI &msg)
{
    if (msg.sType != DUI_MSGTYPE_CLICK)
        return;

    if (msg.pSender->GetName() == L"btn_request_login") {
        OnClickRequestLogin();
    }
}

void LoginView::OnClickRequestLogin()
{
    ClearAllErrorTips();
    auto edit_login_phoneno_ =
        dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(L"edit_login_phoneno"));
    std::wstring phone{edit_login_phoneno_->GetText().GetData()};
    auto edit_login_pwd_ =
        dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(L"edit_login_pwd"));
    std::wstring pwd{edit_login_pwd_->GetText().GetData()};

    if (!IsValidCheckInput(phone, pwd)) {
        return;
    }
    DoRequest(phone, pwd, false);
}

bool LoginView::IsValidCheckInput(const std::wstring &phone, const std::wstring &password)
{
    bool isCheckValid = true;

    if (phone.empty()) {
        ShowErrorTips(errorTipType::phone_number, L"手机号不能为空");
        isCheckValid = false;
    }
    else if (!m_logic_login->IsValidPhoneNumber(phone)) {
        ShowErrorTips(errorTipType::phone_number, L"手机号格式不正确");
        isCheckValid = false;
    }

    if (!m_logic_login->IsValidPassword(password)) {
        ShowErrorTips(errorTipType::password, L"密码格式不正确");
        isCheckValid = false;
    }

    return isCheckValid;
}

void LoginView::ClearAllErrorTips()
{
    auto laberrortipphone_ =
        dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(L"laberrortipphone"));
    laberrortipphone_->SetVisible(false);
    auto labphoneformaterror_ =
        dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(L"labphoneformaterror"));
    labphoneformaterror_->SetVisible(false);
    auto laberrortippwd_ =
        dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(L"laberrortippwd"));

    laberrortippwd_->SetVisible(false);
    auto labpwdformaterror =
        dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(L"labpwdformaterror"));
    labpwdformaterror->SetVisible(false);
}

void LoginView::DoRequest(const std::wstring &phone, const std::wstring &password,
                          bool IsAutoLogin)
{
    ModalWnd::ShowWaiting(m_ownerHwnd);
    auto ansi_phone = ckbase::win32::UnicodeToMBCS(phone);
    auto ansi_password = ckbase::win32::UnicodeToMBCS(password);

    HttpRequestFuncType loginRequestTask = [=](void) -> UnorderedStringMap {
        return m_logic_login->LoginRequest(ansi_phone, ansi_password);
    };

    HttpResponseFuncType loginRetReply = [=](const UnorderedStringMap &ret) {
        ModalWnd::CloseModal();

        if (ret.find("success")->second == "true") {
            ParseForUserInfo(ret);

            SendSuccessNotify(ansi_phone, ansi_password, IsAutoLogin);
            m_bLoginSuccess = true;
        }
        else {
            m_bLoginSuccess = false;
            auto unicodeErrorMsg = ckbase::win32::MBCSToUnicode(ret.at("msg"));
            Toast::Show(unicodeErrorMsg, 2000, m_ownerHwnd);
        }
    };
    //异步等待的语义，这里表示在杂事线程中处理登录任务，
    //异步等待返回结果后，再进行UI线程里的操作，全部过程都是异步的！
    //即不会阻塞UI线程
    Await(ThreadId::Misc, loginRequestTask, loginRetReply);
}

void LoginView::SendSuccessNotify(const std::string &phone, const std::string &password,
                                  bool IsAutoLogin)
{
    auto *pAccountInfo = new AccountInfo;
    auto _optautologinin7days =
        dynamic_cast<ui::COptionUI *>(m_uiManager->FindControl(L"optautologinin7days"));
    assert(_optautologinin7days != nullptr);

    pAccountInfo->_phone = phone;
    m_current_user_info->phone = phone;
    pAccountInfo->_name = m_current_user_info->username;
    pAccountInfo->_tax_no = m_current_user_info->taxNo;
    pAccountInfo->_password = password;
    pAccountInfo->_taxer_type = m_current_user_info->taxType;
    pAccountInfo->_auto_login_flag = _optautologinin7days->IsSelected();
    pAccountInfo->_autologin_end_date = "datetime('now','localtime','+7 days')";
    pAccountInfo->_ent_name = m_current_user_info->entName;

    ui::TNotifyUI notify_one;
    notify_one.sType = Constom_UI_log_Success;
    notify_one.pSender = m_uiManager->FindControl(L"btn_request_login");
    notify_one.wParam = reinterpret_cast<WPARAM>(pAccountInfo);
    notify_one.lParam = IsAutoLogin;

    m_uiManager->SendNotify(notify_one, true);
}

void LoginView::ParseForUserInfo(const UnorderedStringMap &ret)
{
    m_current_user_info->userId = ret.at("userId");
    m_current_user_info->username = ret.at("username");
    m_current_user_info->taxNo = ret.at("taxNo");
    m_current_user_info->taxType = ret.at("taxType");
    m_current_user_info->auditStatus = ret.at("auditStatus");
    m_current_user_info->effectiveDate = ret.at("effectiveDate");
    m_current_user_info->siheAuthStatus = ret.at("siheAuthStatus");
    m_current_user_info->entId = ret.at("entId");
    m_current_user_info->entName = ret.at("entName");
    m_current_user_info->helperStatus = ret.at("helperStatus");
    m_current_user_info->helperAuthEnd = ret.at("helperAuthEnd");
    m_current_user_info->checkTotol = ret.at("checkTotal");
}

void LoginView::ClearAllTips()
{
    ClearAllErrorTips();
    auto edit_login_phoneno_ =
        dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(L"edit_login_phoneno"));
    edit_login_phoneno_->SetText(L"");
    auto edit_login_pwd_ =
        dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(L"edit_login_pwd"));
    edit_login_pwd_->SetText(L"");
}

std::string LoginView::GetCurrentUserName()
{
    return m_current_user_info->username;
}

std::string LoginView::GetCurrentUserCompanyTaxno()
{
    return m_current_user_info->taxNo;
}

std::string LoginView::GetCurrentUserCompanyName()
{
    return m_current_user_info->entName;
}

bool LoginView::IsNeedShowSiheValidDate()
{
    auto siheAuthStatus = m_current_user_info->siheAuthStatus;
    return siheAuthStatus == "01" || siheAuthStatus == "02";
}

bool LoginView::IsNeedShowHelperValidDate()
{
    auto helperStatus = m_current_user_info->helperStatus;
    return helperStatus == "01" || helperStatus == "02";
}

void LoginView::ShowErrorTips(errorTipType type, const std::wstring &errmsg)
{
    if (type == errorTipType::phone_number) {
        auto laberrortipphone_ =
            dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(L"laberrortipphone"));

        laberrortipphone_->SetVisible(true);
        auto labphoneformaterror_ = dynamic_cast<ui::CLabelUI *>(
            m_uiManager->FindControl(L"labphoneformaterror"));
        labphoneformaterror_->SetText(errmsg.data());
        labphoneformaterror_->SetVisible(true);
    }
    else if (type == errorTipType::password) {
        auto laberrortippwd_ =
            dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(L"laberrortippwd"));

        laberrortippwd_->SetVisible(true);

        auto labpwdformaterror =
            dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(L"labpwdformaterror"));
        labpwdformaterror->SetText(errmsg.data());
        labpwdformaterror->SetVisible(true);
    }
}
