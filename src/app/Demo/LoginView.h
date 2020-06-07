#ifndef __INCLUDE_LOGINVIEW_H__
#define __INCLUDE_LOGINVIEW_H__

#pragma once
#include <string>
#include <memory>

#include "UIlib.h"
#include "module/Login.h"

struct UserInfoView {
    std::string phone;
    std::string userId;
    std::string username;
    std::string taxNo;
    std::string taxType;     // 01-小规模纳税人 02 - 一般纳税人
    std::string auditStatus; // 审核状态 :01：待审核  02：通过  03：不通过
    std::string effectiveDate;
    std::string siheAuthStatus; // 00-未授权  01-已授权  02-授权已过期
    std::string entId;
    std::string entName;
    std::string helperStatus; // 助手版授权状态:00未授权  01已授权  02授权已过期
    std::string helperAuthEnd;
    std::string checkTotol;

    void Reset()
    {
        userId.clear();
        username.clear();
        taxNo.clear();
        taxType.clear();
        auditStatus.clear();
        effectiveDate.clear();
        siheAuthStatus.clear();
        entId.clear();
        entName.clear();
        helperStatus.clear();
        helperAuthEnd.clear();
        checkTotol.clear();
    }
};

constexpr auto Constom_UI_log_Success = L"user_login_success_notify";
enum class errorTipType
{
    phone_number,
    password
};
class LoginView : public ui::INotifyUI
{
public:
    LoginView(/* args */ ui::CPaintManagerUI *, HWND hwnd);
    ~LoginView(){};
    void ClearAllTips();
    bool IsLogInSuccess()
    {
        return m_bLoginSuccess;
    }

    /**
     * @brief 外部接口，设置用户已登出
     * @param[in]
     * @return
     */
    void SetLogoutStatus()
    {
        m_bLoginSuccess = false;
        m_current_user_info->Reset();
    }

    std::string GetCurrentUserName();
    std::string GetCurrentUserCompanyTaxno();
    std::string GetCurrentUserCompanyName();
    std::string GetCurrentUserPhone()
    {
        return m_current_user_info->phone;
    }
    /**
     * @brief 是否需要显示思和的有效日期，按01-已授权 02授权已过期的用户来显示
     * @param[in] 无
     * @return
     */
    bool IsNeedShowSiheValidDate();
    /**
     * @brief 是否需要显示助手的有效日期，按01-已授权 02授权已过期的用户来显示
     * @param[in] 无
     * @return
     */
    bool IsNeedShowHelperValidDate();

    std::string GetSiheValidDate()
    {
        return m_current_user_info->effectiveDate;
    }

    std::string GetHelperValidDate()
    {
        return m_current_user_info->helperAuthEnd;
    }

    std::string GetCurrentUserId()
    {
        return m_current_user_info->userId;
    }
    void DoRequest(const std::wstring &phone, const std::wstring &password,
                   bool IsAutoLogin);

private:
    virtual void Notify(ui::TNotifyUI &msg) final;
    void OnClickRequestLogin();
    bool IsValidCheckInput(const std::wstring &phone, const std::wstring &password);
    void ShowErrorTips(errorTipType type, const std::wstring &errmsg);

    void ClearAllErrorTips();

    /**
     * @brief 向后台做登录请求
     * @param[in]
     * @return
     */
    void ParseForUserInfo(const UnorderedStringMap &ret);

    /**
     * @brief 登录成功后向主类发送成功消息
     * @param[in]
     * @return
     */
    void SendSuccessNotify(const std::string &phone, const std::string &password,
                           bool IsAutoLogin);

private:
    ui::CPaintManagerUI *m_uiManager;
    HWND m_ownerHwnd;
    std::unique_ptr<Login> m_logic_login;
    bool m_bLoginSuccess = false;

    std::unique_ptr<UserInfoView> m_current_user_info;
};

#endif // __INCLUDE_LOGINVIEW_H__